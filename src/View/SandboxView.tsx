import React, {useState, useRef, useEffect} from 'react';
import {Canvas} from '@react-three/fiber';
import {PointerLockControls, OrbitControls} from '@react-three/drei';
import {useAction} from '@reatom/npm-react';
import * as THREE from 'three';
import {Grid} from '../Model/Grid';
import {Cell} from '../Model/Cell';
import {useSandboxVM} from '../ViewModel/SandboxViewModelContext';
import {updateObstacle} from "../Model/atoms";
import {EditMode, EditToolbar} from "./components/EditToolbar";
import {FPSMovement} from "./components/FPSMovement";
import {GridDisplay} from "./components/GridDisplay";

const ArrowManipulator: React.FC<{
    axis: 0 | 1 | 2;
    color: string;
    position: [number, number, number];
    onMove: (delta: number) => void;
}> = ({axis, color, position, onMove}) => {
    const [dragging, setDragging] = useState(false);
    const start = useRef<THREE.Vector3>(new THREE.Vector3());

    const dirVec = [
        new THREE.Vector3(1, 0, 0),
        new THREE.Vector3(0, 1, 0),
        new THREE.Vector3(0, 0, 1),
    ][axis];

    const handlePointerDown = (e: any) => {
        e.stopPropagation();
        setDragging(true);
        start.current.copy(e.point);
    };

    const handlePointerMove = (e: any) => {
        if (!dragging) return;
        e.stopPropagation();
        const delta = e.point.dot(dirVec) - start.current.dot(dirVec);
        onMove(delta);
        start.current.copy(e.point);
    };

    const handlePointerUp = (e: any) => {
        e.stopPropagation();
        setDragging(false);
    };

    const rotation: [number, number, number] =
        axis === 0
            ? [0, 0, -Math.PI / 2]
            : axis === 1
                ? [0, 0, 0]
                : [Math.PI / 2, 0, 0];

    const tipPos: [number, number, number] =
        axis === 0
            ? [0.4, 0, 0]
            : axis === 1
                ? [0, 0.4, 0]
                : [0, 0, 0.4];

    return (
        <group position={position}>
            <mesh
                rotation={rotation}
                onPointerDown={handlePointerDown}
                onPointerMove={handlePointerMove}
                onPointerUp={handlePointerUp}
                onPointerOver={e => {
                    e.stopPropagation();
                    document.body.style.cursor = 'pointer';
                }}
                onPointerOut={e => {
                    e.stopPropagation();
                    document.body.style.cursor = 'auto';
                }}
            >
                <cylinderGeometry args={[0.05, 0.05, 0.8, 12]}/>
                <meshStandardMaterial color={color}/>
            </mesh>
            <mesh
                position={tipPos}
                rotation={rotation}
                onPointerDown={handlePointerDown}
                onPointerMove={handlePointerMove}
                onPointerUp={handlePointerUp}
                onPointerOver={e => {
                    e.stopPropagation();
                    document.body.style.cursor = 'pointer';
                }}
                onPointerOut={e => {
                    e.stopPropagation();
                    document.body.style.cursor = 'auto';
                }}
            >
                <coneGeometry args={[0.12, 0.3, 12]}/>
                <meshStandardMaterial color={color}/>
            </mesh>
        </group>
    );
};

const ObstacleObject: React.FC<{
    id: string;
    position: [number, number, number];
    size: [number, number, number];
    color: string;
    onUpdate: (updates: { position?: [number, number, number], size?: [number, number, number] }) => void;
    isSelected: boolean;
    editMode: EditMode;
    onSelect: (id: string) => void;
    isFlightMode: boolean;
}> = ({id, position, size, color, onUpdate, isSelected, editMode, onSelect, isFlightMode}) => {
    const handleMove = (axis: number, delta: number) => {
        const newPosition: [number, number, number] = [...position];
        newPosition[axis] += delta;
        onUpdate({position: newPosition});
    };

    const handleScale = (axis: number, delta: number) => {
        const newSize: [number, number, number] = [...size];
        newSize[axis] = Math.max(0.1, size[axis] + delta);
        onUpdate({size: newSize});
    };

    return (
        <group position={position}>
            <mesh
                userData={{obstacleId: id}}
                onClick={e => {
                    if (isFlightMode) return;
                    e.stopPropagation();
                    onSelect(id);
                }}
                onPointerOver={e => {
                    e.stopPropagation();
                    document.body.style.cursor = 'pointer';
                }}
                onPointerOut={e => {
                    e.stopPropagation();
                    document.body.style.cursor = 'auto';
                }}
            >
                <boxGeometry args={size}/>
                <meshStandardMaterial color={color}/>
            </mesh>
            {isSelected && editMode === 'move' && (
                <>
                    <ArrowManipulator
                        axis={0}
                        color="red"
                        position={[size[0] / 2 + 0.6, 0, 0]}
                        onMove={delta => handleMove(0, delta)}
                    />
                    <ArrowManipulator
                        axis={1}
                        color="green"
                        position={[0, size[1] / 2 + 0.6, 0]}
                        onMove={delta => handleMove(1, delta)}
                    />
                    <ArrowManipulator
                        axis={2}
                        color="blue"
                        position={[0, 0, size[2] / 2 + 0.6]}
                        onMove={delta => handleMove(2, delta)}
                    />
                </>
            )}
            {isSelected && editMode === 'scale' && (
                <>
                    <ArrowManipulator
                        axis={0}
                        color="red"
                        position={[size[0] / 2 + 0.6, 0, 0]}
                        onMove={(delta) => handleScale(0, delta)}
                    />
                    <ArrowManipulator
                        axis={1}
                        color="green"
                        position={[0, size[1] / 2 + 0.6, 0]}
                        onMove={(delta) => handleScale(1, delta)}
                    />
                    <ArrowManipulator
                        axis={2}
                        color="blue"
                        position={[0, 0, size[2] / 2 + 0.6]}
                        onMove={(delta) => handleScale(2, delta)}
                    />
                </>
            )}
        </group>
    );
};

const SandboxView: React.FC = () => {
    const updateObstacleById = useAction(updateObstacle);
    const [selectedObstacle, setSelectedObstacle] = useState<string | null>(null);
    const [getPickMode, setgetPickMode] = useState<'start' | 'end' | null>(null);
    const [showGrid, setShowGrid] = useState(true);
    const [showCostText, setShowCostText] = useState(false);
    const [grid, setGrid] = useState<Grid | null>(null);
    const [showCanvas, setShowCanvas] = useState(true);

    const GRID_SIZE_X = 20;
    const GRID_SIZE_Y = 20;
    const GRID_SIZE_Z = 3;
    const CELL_SIZE = 1;

    const sandboxViewModel = useSandboxVM();

    const handleObstacleUpdate = (id: string, updates: {
        position?: [number, number, number],
        size?: [number, number, number]
    }) => {
        updateObstacleById(id, updates);
    };

    const handleSelectObstacle = (id: string) => {
        setSelectedObstacle(id);
        sandboxViewModel.setEditMode('move');
    };

    const handleSceneClick = (e: any) => {
        if (e.eventObject === e.currentTarget) {
            if (!sandboxViewModel.assertFlightMode) {
                sandboxViewModel.selectObstacle(null);
            }
        }

        if (sandboxViewModel.getPickMode) {
            sandboxViewModel.handleGridClick(e.point);
        }
    };

    useEffect(() => {
        if (!selectedObstacle) return;
        const handleKeyDown = (e: KeyboardEvent) => {
            let axis: 0 | 1 | 2 | null = null;
            let delta = 0;
            const step = 0.1;
            const obstacle = sandboxViewModel.getObstacles.find(o => o.id === selectedObstacle);
            if (!obstacle) return;
            if (!e.shiftKey) {
                if (e.key === 'ArrowUp') {
                    axis = 1;
                    delta = step;
                } else if (e.key === 'ArrowDown') {
                    axis = 1;
                    delta = -step;
                } else if (e.key === 'ArrowLeft') {
                    axis = 0;
                    delta = -step;
                } else if (e.key === 'ArrowRight') {
                    axis = 0;
                    delta = step;
                } else if (e.key === 'PageUp') {
                    axis = 2;
                    delta = step;
                } else if (e.key === 'PageDown') {
                    axis = 2;
                    delta = -step;
                }
                if (axis !== null) {
                    const pos = [...obstacle.position];
                    pos[axis] += delta;
                    updateObstacleById(selectedObstacle, {position: pos as [number, number, number]});
                }
            } else {
                if (e.key === 'ArrowUp') {
                    axis = 1;
                    delta = step;
                } else if (e.key === 'ArrowDown') {
                    axis = 1;
                    delta = -step;
                } else if (e.key === 'ArrowLeft') {
                    axis = 0;
                    delta = -step;
                } else if (e.key === 'ArrowRight') {
                    axis = 0;
                    delta = step;
                } else if (e.key === 'PageUp') {
                    axis = 2;
                    delta = step;
                } else if (e.key === 'PageDown') {
                    axis = 2;
                    delta = -step;
                }
                if (axis !== null) {
                    const size = [...obstacle.size];
                    size[axis] = Math.max(0.1, size[axis] + delta);
                    updateObstacleById(selectedObstacle, {size: size as [number, number, number]});
                }
            }
        };
        window.addEventListener('keydown', handleKeyDown);
        return () => window.removeEventListener('keydown', handleKeyDown);
    }, [selectedObstacle, updateObstacleById, sandboxViewModel.getObstacles]);

    React.useEffect(() => {
        const handleKeyDown = (e: KeyboardEvent) => {
            if (e.key === 'Escape' && getPickMode) {
                setgetPickMode(null);
            }
        };
        window.addEventListener('keydown', handleKeyDown);
        return () => window.removeEventListener('keydown', handleKeyDown);
    }, [getPickMode]);

    useEffect(() => {
        const offset: [number, number, number] = [0, 0, 0];
        setGrid(new Grid(GRID_SIZE_X, GRID_SIZE_Y, GRID_SIZE_Z, CELL_SIZE, CELL_SIZE, offset));
    }, []);

    useEffect(() => {
        if (grid) {
            sandboxViewModel.recalculateGridCosts();
        }
    });

    const getCellCenterVector = React.useCallback((cell: Cell): THREE.Vector3 | null => {
        if (!grid || !cell) return null;
        const {x, y, z} = cell.getCoords();
        const [cx, cy, cz] = grid.getCellCenter(x, y, z);
        return new THREE.Vector3(cx, cy, cz);
    }, [grid]);

    useEffect(() => {
        if (!sandboxViewModel.assertFlightMode && document.pointerLockElement) {
            document.exitPointerLock();
            setTimeout(() => setShowCanvas(false), 100);
            setTimeout(() => setShowCanvas(true), 120);
        } else if (sandboxViewModel.assertFlightMode && !showCanvas) {
            setShowCanvas(true);
        }
    }, [sandboxViewModel.assertFlightMode, showCanvas]);

    return (
        <div style={{position: 'relative', width: '100%', height: '100%'}}>
            <div style={{position: 'absolute', zIndex: 2, left: 20, top: 20}}>
                <div>
                    <button onClick={sandboxViewModel.toggleFlightMode}>
                        {sandboxViewModel.assertFlightMode ? 'Режим редактирования' : 'Режим полёта'}
                    </button>
                    {!sandboxViewModel.assertFlightMode &&
                        <button onClick={sandboxViewModel.addObstacle}>Добавить препятствие</button>}
                </div>
                {!sandboxViewModel.assertFlightMode && sandboxViewModel.getSelectedObstacleId && (
                    <EditToolbar
                        editMode={sandboxViewModel.getEditMode}
                        onModeChange={sandboxViewModel.setEditMode}
                        onDelete={sandboxViewModel.removeSelectedObstacle}
                        onDuplicate={sandboxViewModel.duplicateSelectedObstacle}
                        onReset={sandboxViewModel.resetSelectedObstacle}
                    />
                )}
                <div style={{marginTop: 16}}>
                    <button onClick={() => sandboxViewModel.saveState()}>Сохранить состояние</button>
                    <label style={{marginLeft: 8}}>
                        Загрузить состояние:
                        <input
                            type="file"
                            accept=".json"
                            onChange={e => {
                                const file = e.target.files?.[0];
                                if (file) {
                                    sandboxViewModel.loadState(file);
                                }
                            }}
                            style={{display: 'none'}}
                        />
                        <button
                            onClick={() => (document.querySelector('input[type="file"]') as HTMLInputElement)?.click()}
                            style={{marginLeft: 8}}
                        >
                            Выбрать файл
                        </button>
                    </label>
                </div>
            </div>
            <div style={{position: 'absolute', zIndex: 2, left: 20, top: 120}}>
                <button
                    onClick={() => sandboxViewModel.setPickMode('start')}
                    style={{
                        background: sandboxViewModel.getPickMode === 'start' ? '#4CAF50' : '#fff',
                        color: sandboxViewModel.getPickMode === 'start' ? 'white' : 'black',
                        border: '1px solid #ccc',
                        padding: '8px 16px',
                        borderRadius: '4px',
                        cursor: 'pointer'
                    }}
                >
                    {sandboxViewModel.getPickMode === 'start' ? 'Выберите старт (кликните на сетку)' : 'Выбрать старт'}
                </button>
                <button
                    onClick={() => sandboxViewModel.setPickMode('end')}
                    style={{
                        background: sandboxViewModel.getPickMode === 'end' ? '#4CAF50' : '#fff',
                        color: sandboxViewModel.getPickMode === 'end' ? 'white' : 'black',
                        border: '1px solid #ccc',
                        padding: '8px 16px',
                        borderRadius: '4px',
                        cursor: 'pointer'
                    }}
                >
                    {sandboxViewModel.getPickMode === 'end' ? 'Выберите финиш (кликните на сетку)' : 'Выбрать финиш'}
                </button>
                <button onClick={sandboxViewModel.calculatePath}
                        disabled={!sandboxViewModel.getStartCell || !sandboxViewModel.getEndCell}>Построить путь
                </button>
                <button
                    onClick={() => setShowGrid(v => !v)}>{showGrid ? 'Скрыть сетку' : 'Показать сетку'}</button>
                <button onClick={() => setShowCostText(v => !v)}>
                    {showCostText ? 'Скрыть стоимость' : 'Показать стоимость'}
                </button>
                <button onClick={() => {
                    if (grid) {
                        sandboxViewModel.recalculateGridCosts();
                    }
                }}>
                    Пересчитать стоимости
                </button>
                {sandboxViewModel.getPickMode && (
                    <div style={{
                        marginTop: 8,
                        padding: 8,
                        background: '#fff3cd',
                        border: '1px solid #ffeaa7',
                        borderRadius: 4,
                        color: '#856404'
                    }}>
                        Режим выбора активен. Кликните на сетку для
                        выбора {getPickMode === 'start' ? 'начальной' : 'конечной'} точки.
                        <button
                            onClick={() => sandboxViewModel.setPickMode(null)}
                            style={{
                                marginLeft: 8,
                                background: '#dc3545',
                                color: 'white',
                                border: 'none',
                                padding: '4px 8px',
                                borderRadius: '4px',
                                cursor: 'pointer',
                                fontSize: '12px'
                            }}
                        >
                            Отменить
                        </button>
                    </div>
                )}
            </div>
            {showCanvas && (
                <Canvas
                    key={sandboxViewModel.assertFlightMode ? 'flight' : 'edit'}
                    camera={{position: [0, GRID_SIZE_Y * 1.5, 0], fov: 60}}
                    style={{width: '100%', height: '100%'}}
                >
                    <ambientLight intensity={0.7}/>
                    <pointLight position={[10, 10, 10]}/>
                    {sandboxViewModel.assertFlightMode ? (
                        <>
                            <PointerLockControls/>
                            <FPSMovement enabled={true}/>
                        </>
                    ) : (
                        <OrbitControls/>
                    )}
                    <gridHelper args={[GRID_SIZE_X, GRID_SIZE_X]} position={[GRID_SIZE_X / 2, 0, GRID_SIZE_Y / 2]}/>
                    <axesHelper args={[5]}/>
                    {sandboxViewModel.getPickMode && (
                        <group>
                            <mesh
                                position={[0, 0, 0]}
                                rotation={[-Math.PI / 2, 0, 0]}
                                onClick={handleSceneClick}
                            >
                                <planeGeometry args={[1000, 1000]}/>
                                <meshBasicMaterial transparent opacity={0}/>
                            </mesh>
                            <mesh
                                position={[0, 0, 0]}
                                rotation={[0, 0, 0]}
                                onClick={handleSceneClick}
                            >
                                <planeGeometry args={[1000, 1000]}/>
                                <meshBasicMaterial transparent opacity={0}/>
                            </mesh>
                            <mesh
                                position={[0, 0, 0]}
                                rotation={[0, Math.PI / 2, 0]}
                                onClick={handleSceneClick}
                            >
                                <planeGeometry args={[1000, 1000]}/>
                                <meshBasicMaterial transparent opacity={0}/>
                            </mesh>
                        </group>
                    )}
                    {sandboxViewModel.getObstacles.map((obstacle) => (
                        <ObstacleObject
                            key={obstacle.id}
                            id={obstacle.id}
                            position={obstacle.position}
                            size={obstacle.size}
                            color={obstacle.color}
                            onUpdate={(updates) => handleObstacleUpdate(obstacle.id, updates)}
                            isSelected={selectedObstacle === obstacle.id}
                            editMode={sandboxViewModel.getEditMode}
                            onSelect={handleSelectObstacle}
                            isFlightMode={sandboxViewModel.assertFlightMode}
                        />
                    ))}
                    {grid && showGrid && <GridDisplay grid={grid} showCostText={showCostText}/>}
                    {sandboxViewModel.getStartCell && (() => {
                        const pos = getCellCenterVector(sandboxViewModel.getStartCell);
                        if (!pos) return null;
                        return (
                            <mesh position={[pos.x, pos.y, pos.z]}>
                                <sphereGeometry args={[0.15]}/>
                                <meshStandardMaterial color="green"/>
                            </mesh>
                        );
                    })()}
                    {sandboxViewModel.getEndCell && (() => {
                        const pos = getCellCenterVector(sandboxViewModel.getEndCell);
                        if (!pos) return null;
                        return (
                            <mesh position={[pos.x, pos.y, pos.z]}>
                                <sphereGeometry args={[0.15]}/>
                                <meshStandardMaterial color="blue"/>
                            </mesh>
                        );
                    })()}
                    {sandboxViewModel.getPath && (
                        <group>
                            {sandboxViewModel.getPath.map(([x, y, z], idx) => {
                                if (!grid) return null;
                                return (
                                    <mesh key={idx} position={[x, y, z]}>
                                        <sphereGeometry args={[0.03]}/>
                                        <meshStandardMaterial color="red"/>
                                    </mesh>
                                );
                            })}
                        </group>
                    )}
                </Canvas>
            )}
            <div style={{
                position: 'absolute',
                left: '50%',
                top: '50%',
                transform: 'translate(-50%, -50%)',
                zIndex: 10,
                pointerEvents: 'none',
                width: 12,
                height: 12,
                borderRadius: '50%',
                background: 'rgba(0,0,0,0.7)',
                border: '2px solid white',
                boxSizing: 'border-box',
            }}/>
        </div>
    );
};

export default SandboxView; 