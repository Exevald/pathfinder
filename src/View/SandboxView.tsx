import React, {useState, useRef, useEffect} from 'react';
import {Canvas, useFrame, useThree} from '@react-three/fiber';
import {PointerLockControls, OrbitControls, Text} from '@react-three/drei';
import {useAtom, useAction} from '@reatom/npm-react';
import {
    modeAtom,
    obstaclesAtom,
    addObstacle,
    removeObstacle,
    updateObstacle,
    saveSandboxState,
    loadSandboxState
} from '../Model/atoms';
import * as THREE from 'three';
import {Grid, PathPoint} from '../Model/Grid';
import {Cell} from '../Model/Cell';
import type {Obstacle} from '../Model/atoms';

type EditMode = 'move' | 'rotate' | 'scale' | null;

const FPSMovement: React.FC<{ enabled: boolean }> = ({enabled}) => {
    const {camera} = useThree();
    const move = useRef({forward: false, backward: false, left: false, right: false, up: false});
    const speed = 0.1;

    React.useEffect(() => {
        if (!enabled) return;

        const handleKeyDown = (e: KeyboardEvent) => {
            switch (e.code) {
                case 'KeyW':
                    move.current.forward = true;
                    break;
                case 'KeyS':
                    move.current.backward = true;
                    break;
                case 'KeyA':
                    move.current.left = true;
                    break;
                case 'KeyD':
                    move.current.right = true;
                    break;
                case 'Space':
                    move.current.up = true;
                    break;
            }
        };
        const handleKeyUp = (e: KeyboardEvent) => {
            switch (e.code) {
                case 'KeyW':
                    move.current.forward = false;
                    break;
                case 'KeyS':
                    move.current.backward = false;
                    break;
                case 'KeyA':
                    move.current.left = false;
                    break;
                case 'KeyD':
                    move.current.right = false;
                    break;
                case 'Space':
                    move.current.up = false;
                    break;
            }
        };
        window.addEventListener('keydown', handleKeyDown);
        window.addEventListener('keyup', handleKeyUp);
        return () => {
            window.removeEventListener('keydown', handleKeyDown);
            window.removeEventListener('keyup', handleKeyUp);
        };
    }, [enabled]);

    useFrame(() => {
        if (!enabled) return;
        const direction = new THREE.Vector3();
        camera.getWorldDirection(direction);

        const moveVector = new THREE.Vector3();
        if (move.current.forward) moveVector.add(direction);
        if (move.current.backward) moveVector.sub(direction);

        const right = new THREE.Vector3();
        right.crossVectors(camera.up, direction).normalize();
        if (move.current.right) moveVector.sub(right);
        if (move.current.left) moveVector.add(right);

        if (move.current.up) moveVector.y += 1;

        if (moveVector.lengthSq() > 0) {
            moveVector.normalize().multiplyScalar(speed);
            camera.position.add(moveVector);
        }
    });

    return null;
};

const getArrowTipPosition = (axis: 0 | 1 | 2, size: [number, number, number]) => {
    if (axis === 0) return [0.5, 0, 0]; // X
    if (axis === 1) return [0, 0.5, 0]; // Y
    return [0, 0, 0.5]; // Z
};

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

const EditToolbar: React.FC<{
    editMode: EditMode;
    onModeChange: (mode: EditMode) => void;
    onDelete: () => void;
    onDuplicate: () => void;
    onReset: () => void;
}> = ({editMode, onModeChange, onDelete, onDuplicate, onReset}) => {
    return (
        <div style={{
            display: 'flex',
            flexDirection: 'column',
            gap: '8px',
            padding: '8px',
            background: '#f0f0f0',
            borderRadius: '4px',
            marginBottom: '16px'
        }}>
            <div style={{display: 'flex', gap: '8px'}}>
                <button
                    onClick={() => onModeChange('move')}
                    style={{
                        background: editMode === 'move' ? '#4CAF50' : '#fff',
                        color: editMode === 'move' ? 'white' : 'black',
                        border: '1px solid #ccc',
                        padding: '8px 16px',
                        borderRadius: '4px',
                        cursor: 'pointer'
                    }}
                >
                    Переместить
                </button>
                <button
                    onClick={() => onModeChange('scale')}
                    style={{
                        background: editMode === 'scale' ? '#4CAF50' : '#fff',
                        color: editMode === 'scale' ? 'white' : 'black',
                        border: '1px solid #ccc',
                        padding: '8px 16px',
                        borderRadius: '4px',
                        cursor: 'pointer'
                    }}
                >
                    Изменить размер
                </button>
            </div>
            <div style={{display: 'flex', gap: '8px'}}>
                <button
                    onClick={onDuplicate}
                    style={{
                        background: '#2196F3',
                        color: 'white',
                        border: 'none',
                        padding: '8px 16px',
                        borderRadius: '4px',
                        cursor: 'pointer'
                    }}
                >
                    Дублировать
                </button>
                <button
                    onClick={onReset}
                    style={{
                        background: '#FF9800',
                        color: 'white',
                        border: 'none',
                        padding: '8px 16px',
                        borderRadius: '4px',
                        cursor: 'pointer'
                    }}
                >
                    Сбросить
                </button>
                <button
                    onClick={onDelete}
                    style={{
                        background: '#ff4444',
                        color: 'white',
                        border: 'none',
                        padding: '8px 16px',
                        borderRadius: '4px',
                        cursor: 'pointer'
                    }}
                >
                    Удалить
                </button>
            </div>
        </div>
    );
};

const SandboxView: React.FC = () => {
    const [mode] = useAtom(modeAtom);
    const [obstacles] = useAtom(obstaclesAtom);
    const addNewObstacle = useAction(addObstacle);
    const removeObstacleById = useAction(removeObstacle);
    const updateObstacleById = useAction(updateObstacle);
    const saveState = useAction(saveSandboxState);
    const loadState = useAction(loadSandboxState);
    const [isFlightMode, setIsFlightMode] = useState(false);
    const [selectedObstacle, setSelectedObstacle] = useState<string | null>(null);
    const [editMode, setEditMode] = useState<EditMode>(null);
    const [pickMode, setPickMode] = useState<'start' | 'end' | null>(null);
    const [showGrid, setShowGrid] = useState(true);
    const [showCostText, setShowCostText] = useState(false);
    const [startCell, setStartCell] = useState<Cell | null>(null);
    const [endCell, setEndCell] = useState<Cell | null>(null);
    const [path, setPath] = useState<PathPoint[] | null>(null);
    const [grid, setGrid] = useState<Grid | null>(null);
    const [showCanvas, setShowCanvas] = useState(true);

    const GRID_SIZE_X = 20;
    const GRID_SIZE_Y = 20;
    const GRID_SIZE_Z = 3;
    const CELL_SIZE = 1;

    const handleAddObstacle = () => {
        addNewObstacle({
            position: [0, 0, 0],
            size: [1, 1, 1],
            color: '#' + Math.floor(Math.random() * 16777215).toString(16)
        });
    };

    const handleObstacleUpdate = (id: string, updates: {
        position?: [number, number, number],
        size?: [number, number, number]
    }) => {
        updateObstacleById(id, updates);
    };

    const toggleMode = () => {
        setIsFlightMode(!isFlightMode);
        setSelectedObstacle(null);
        setEditMode(null);
    };

    const handleDuplicateObstacle = () => {
        if (!selectedObstacle) return;
        const obstacle = obstacles.find(o => o.id === selectedObstacle);
        if (!obstacle) return;

        addNewObstacle({
            position: [
                obstacle.position[0] + 1,
                obstacle.position[1],
                obstacle.position[2]
            ],
            size: [...obstacle.size],
            color: obstacle.color
        });
    };

    const handleResetObstacle = () => {
        if (!selectedObstacle) return;
        updateObstacleById(selectedObstacle, {
            position: [0, 0, 0],
            size: [1, 1, 1]
        });
    };

    const handleSelectObstacle = (id: string) => {
        setSelectedObstacle(id);
        setEditMode('move');
    };

    React.useEffect(() => {
        if (!selectedObstacle) return;
        const handleKeyDown = (e: KeyboardEvent) => {
            let axis: 0 | 1 | 2 | null = null;
            let delta = 0;
            const step = 0.1;
            const obstacle = obstacles.find(o => o.id === selectedObstacle);
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
    }, [selectedObstacle, updateObstacleById, obstacles]);

    useEffect(() => {
        setGrid(new Grid(GRID_SIZE_X, GRID_SIZE_Y, GRID_SIZE_Z, CELL_SIZE, CELL_SIZE, [0, 0, 0]));
    }, []);

    function recalculateGridCosts(grid: Grid, obstacles: Obstacle[]) {
        for (let x = 0; x < grid.getGridSizeX(); ++x) {
            for (let y = 0; y < grid.getGridSizeY(); ++y) {
                for (let z = 0; z < grid.getGridSizeZ(); ++z) {
                    const cell = grid.getCellByIndices(x, y, z);
                    if (!cell) continue;
                    const [cx, cy, cz] = grid.getCellCenter(x, y, z);
                    let isInside = false;
                    for (const obs of obstacles) {
                        const [ox, oy, oz] = obs.position;
                        const [sx, sy, sz] = obs.size;
                        if (
                            cx >= ox - sx / 2 && cx <= ox + sx / 2 &&
                            cy >= oy - sy / 2 && cy <= oy + sy / 2 &&
                            cz >= oz - sz / 2 && cz <= oz + sz / 2
                        ) {
                            isInside = true;
                            break;
                        }
                    }
                    cell.setCost(isInside ? 254 : 50);
                }
            }
        }
    }

    useEffect(() => {
        if (grid) {
            recalculateGridCosts(grid, obstacles);
        }
    }, [obstacles, grid]);

    const getCellByPosition = (pos: THREE.Vector3): Cell | null => {
        if (!grid) return null;
        const offset = grid.getOffset();
        const cellLength = grid.getCellLength();
        const layerHeight = grid.getLayerLength();
        const ix = Math.floor((pos.x - offset[0]) / cellLength);
        const iz = Math.floor((pos.z - offset[2]) / layerHeight);
        const iy = Math.floor((pos.y - offset[1]) / cellLength);
        return grid.getCellByIndices(ix, iy, iz);
    };

    const getCellCenterVector = (cell: Cell): THREE.Vector3 | null => {
        if (!grid || !cell) return null;
        const {x, y, z} = cell.getCoords();
        const [cx, cy, cz] = grid.getCellCenter(x, y, z);
        return new THREE.Vector3(cx, cy, cz);
    };

    const handleSandboxClick = (e: any) => {
        if (!pickMode) return;
        let camera = e.camera;
        let scene = e.scene;
        if (!camera || !scene) return;
        const mouse = new THREE.Vector2(0, 0);
        const raycaster = new THREE.Raycaster();
        raycaster.setFromCamera(mouse, camera);
        const intersects = raycaster.intersectObjects(scene.children, true);
        if (intersects.length === 0) return;
        const point = intersects[0].point;
        const cell = getCellByPosition(point);
        if (!cell) return;
        if (pickMode === 'start') setStartCell(cell);
        if (pickMode === 'end') setEndCell(cell);
        setPickMode(null);
    };

    const calcPath = () => {
        if (!grid || !startCell || !endCell) return;
        setPath(grid.findPath(startCell, endCell));
    };

    useEffect(() => {
        if (!isFlightMode && document.pointerLockElement) {
            document.exitPointerLock();
            setTimeout(() => setShowCanvas(false), 100);
            setTimeout(() => setShowCanvas(true), 120);
        } else if (isFlightMode && !showCanvas) {
            setShowCanvas(true);
        }
    }, [isFlightMode, showCanvas]);

    return (
        <div style={{position: 'relative', width: '100%', height: '100%'}}>
            <div style={{position: 'absolute', zIndex: 2, left: 20, top: 20}}>
                <button onClick={toggleMode}>
                    {isFlightMode ? 'Режим редактирования' : 'Режим полёта'}
                </button>
                {!isFlightMode && (
                    <>
                        <button onClick={handleAddObstacle}>Добавить препятствие</button>
                        <EditToolbar
                            editMode={editMode}
                            onModeChange={setEditMode}
                            onDelete={selectedObstacle ? () => removeObstacleById(selectedObstacle) : () => {
                            }}
                            onDuplicate={selectedObstacle ? handleDuplicateObstacle : () => {
                            }}
                            onReset={selectedObstacle ? handleResetObstacle : () => {
                            }}
                        />
                    </>
                )}
                <div style={{marginTop: 16}}>
                    <button onClick={() => saveState()}>Сохранить состояние</button>
                    <label style={{marginLeft: 8}}>
                        Загрузить состояние:
                        <input
                            type="file"
                            accept=".json"
                            onChange={e => {
                                const file = e.target.files?.[0];
                                if (file) loadState(file);
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
                <button onClick={() => setPickMode('start')}>Выбрать старт</button>
                <button onClick={() => setPickMode('end')}>Выбрать финиш</button>
                <button onClick={calcPath} disabled={!startCell || !endCell}>Построить путь</button>
                <button onClick={() => setShowGrid(v => !v)}>{showGrid ? 'Скрыть сетку' : 'Показать сетку'}</button>
                <button onClick={() => setShowCostText(v => !v)}>
                    {showCostText ? 'Скрыть стоимость' : 'Показать стоимость'}
                </button>
                <button onClick={() => {
                    if (grid) recalculateGridCosts(grid, obstacles);
                }}>
                    Пересчитать стоимости
                </button>
            </div>
            {showCanvas && (
                <Canvas
                    key={isFlightMode ? 'flight' : 'edit'}
                    camera={{position: [GRID_SIZE_X / 2, GRID_SIZE_Y * 1.5, GRID_SIZE_Z * 2], fov: 60}}
                    style={{width: '100%', height: '100%'}}
                    onPointerDown={handleSandboxClick}
                >
                    <ambientLight intensity={0.7}/>
                    <pointLight position={[10, 10, 10]}/>
                    {isFlightMode ? (
                        <>
                            <PointerLockControls/>
                            <FPSMovement enabled={true}/>
                        </>
                    ) : (
                        <OrbitControls/>
                    )}
                    <gridHelper args={[20, 20]}/>
                    <axesHelper args={[5]}/>
                    {obstacles.map((obstacle) => (
                        <ObstacleObject
                            key={obstacle.id}
                            id={obstacle.id}
                            position={obstacle.position}
                            size={obstacle.size}
                            color={obstacle.color}
                            onUpdate={(updates) => handleObstacleUpdate(obstacle.id, updates)}
                            isSelected={selectedObstacle === obstacle.id}
                            editMode={editMode}
                            onSelect={handleSelectObstacle}
                            isFlightMode={isFlightMode}
                        />
                    ))}
                    {grid && showGrid && (() => {
                        const cellLength = grid.getCellLength();
                        const layerHeight = grid.getLayerLength();
                        const gridSizeX = grid.getGridSizeX();
                        const gridSizeY = grid.getGridSizeY();
                        const gridSizeZ = grid.getGridSizeZ();
                        const offset = grid.getOffset ? grid.getOffset() : [0, 0, 0];
                        const allCells = [];
                        for (let x = 0; x < gridSizeX; ++x) {
                            for (let y = 0; y < gridSizeY; ++y) {
                                for (let z = 0; z < gridSizeZ; ++z) {
                                    const cx = offset[0] + (x + 0.5) * cellLength;
                                    const cy = offset[1] + (y + 0.5) * cellLength;
                                    const cz = offset[2] + (z + 0.5) * layerHeight;
                                    const cell = grid.getCellByIndices(x, y, z);
                                    if (cell) {
                                        allCells.push(
                                            <CellEdges
                                                key={`cell-${x}-${y}-${z}`}
                                                center={[cx, cz, cy]}
                                                size={[cellLength, cellLength, layerHeight]}
                                                cost={cell.getCost()}
                                                showText={showCostText}
                                            />
                                        );
                                    }
                                }
                            }
                        }
                        return <group>{allCells}</group>;
                    })()}
                    {startCell && (() => {
                        const pos = getCellCenterVector(startCell);
                        if (!pos) return null;
                        return (
                            <mesh position={[pos.x, pos.y, pos.z]}>
                                <sphereGeometry args={[0.15]}/>
                                <meshStandardMaterial color="green"/>
                            </mesh>
                        );
                    })()}
                    {endCell && (() => {
                        const pos = getCellCenterVector(endCell);
                        if (!pos) return null;
                        return (
                            <mesh position={[pos.x, pos.y, pos.z]}>
                                <sphereGeometry args={[0.15]}/>
                                <meshStandardMaterial color="blue"/>
                            </mesh>
                        );
                    })()}
                    {path && (
                        <group>
                            {path.map(([x, y, z], idx) => {
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

function getBoxEdges(center: [number, number, number], size: [number, number, number]) {
    const [cx, cy, cz] = center;
    const [sx, sy, sz] = size;
    const hx = sx / 2, hy = sy / 2, hz = sz / 2;
    const v = [
        [cx - hx, cy - hy, cz - hz],
        [cx + hx, cy - hy, cz - hz],
        [cx + hx, cy + hy, cz - hz],
        [cx - hx, cy + hy, cz - hz],
        [cx - hx, cy - hy, cz + hz],
        [cx + hx, cy - hy, cz + hz],
        [cx + hx, cy + hy, cz + hz],
        [cx - hx, cy + hy, cz + hz],
    ];
    const edges = [
        [v[0], v[1]], [v[1], v[2]], [v[2], v[3]], [v[3], v[0]],
        [v[4], v[5]], [v[5], v[6]], [v[6], v[7]], [v[7], v[4]],
        [v[0], v[4]], [v[1], v[5]], [v[2], v[6]], [v[3], v[7]],
    ];
    return edges.flat();
}

const CellEdges: React.FC<{
    center: [number, number, number],
    size: [number, number, number],
    cost: number,
    showText: boolean
}> = ({center, size, cost, showText}) => {
    const points = getBoxEdges(center, size).map(([x, y, z]) => new THREE.Vector3(x, y, z));
    return (
        <group>
            <lineSegments>
                <bufferGeometry>
                    <bufferAttribute
                        attach="attributes-position"
                        args={[
                            new Float32Array(points.flatMap(p => [p.x, p.y, p.z])),
                            3
                        ]}
                    />
                </bufferGeometry>
                <lineBasicMaterial color="red" linewidth={1.5}/>
            </lineSegments>
            {showText && (
                <Text
                    position={[center[0], center[1], center[2]]}
                    fontSize={0.05}
                    color="black"
                    anchorX="center"
                    anchorY="middle"
                >
                    {cost.toFixed(1)}
                </Text>
            )}
        </group>
    );
};

export default SandboxView; 