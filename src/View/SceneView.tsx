import React, {useState, useRef, useEffect} from 'react';
import {Canvas, useFrame, useThree} from '@react-three/fiber';
import {PointerLockControls} from '@react-three/drei';
import {usePathfindingVM} from '../ViewModel/PathfindingViewModel';
import * as THREE from 'three';

const FPSMovement: React.FC<{ enabled: boolean }> = ({enabled}) => {
    const {camera} = useThree();
    const move = useRef({forward: false, backward: false, left: false, right: false, up: false});
    const speed = 0.2;

    useEffect(() => {
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

const CellEdges: React.FC<{ center: [number, number, number], size: [number, number, number] }> = ({center, size}) => {
    const points = getBoxEdges(center, size).map(([x, y, z]) => new THREE.Vector3(x, y, z));
    return (
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
    );
};

const SceneView: React.FC = () => {
    const {
        objObject, startCell, endCell, path,
        setStart, setEnd, calcPath,
        getCellByPosition, getCellCenterVector, grid
    } = usePathfindingVM();

    const [controlsEnabled, setControlsEnabled] = useState(false);
    const [pickMode, setPickMode] = useState<'start' | 'end' | null>(null);
    const canvasRef = useRef<HTMLDivElement>(null);

    useEffect(() => {
        const handlePointerLockChange = () => {
            const isLocked = document.pointerLockElement !== null;
            if (!isLocked) setControlsEnabled(false);
        };
        document.addEventListener('pointerlockchange', handlePointerLockChange);
        return () => {
            document.removeEventListener('pointerlockchange', handlePointerLockChange);
        };
    }, []);

    const handleEnter3D = () => {
        setControlsEnabled(true);
    };

    const handleSceneClick = (e: any) => {
        if (!pickMode) return;
        const point = e.point as THREE.Vector3;
        if (!point) return;
        const cell = getCellByPosition(point);
        if (!cell) return;
        if (pickMode === 'start') setStart(cell);
        if (pickMode === 'end') setEnd(cell);
        setPickMode(null);
    };

    return (
        <div style={{position: 'relative', width: '100%', height: '100%'}} ref={canvasRef}>
            <div style={{position: 'absolute', zIndex: 2, left: 20, top: 20}}>
                <button onClick={() => setPickMode('start')}>Выбрать старт</button>
                <button onClick={() => setPickMode('end')}>Выбрать финиш</button>
                <button onClick={calcPath} disabled={!startCell || !endCell}>Построить путь</button>
            </div>
            {!controlsEnabled && (
                <button
                    style={{
                        position: 'absolute',
                        left: '50%',
                        top: '50%',
                        transform: 'translate(-50%, -50%)',
                        zIndex: 2,
                        padding: '1em 2em',
                        fontSize: '1.2em',
                        cursor: 'pointer',
                    }}
                    onClick={handleEnter3D}
                >
                    Войти в режим управления камерой
                </button>
            )}
            <Canvas
                camera={{position: [10, 10, 10], fov: 60}}
                style={{width: '100%', height: '100%'}}
            >
                <ambientLight intensity={0.7}/>
                {controlsEnabled && <PointerLockControls/>}
                <FPSMovement enabled={controlsEnabled}/>
                {pickMode && (
                    <mesh
                        position={[0, 0, 0]}
                        rotation={[-Math.PI / 2, 0, 0]}
                        onClick={handleSceneClick}
                    >
                        <planeGeometry args={[100, 100]}/>
                        <meshBasicMaterial transparent opacity={0}/>
                    </mesh>
                )}
                {objObject && <primitive object={objObject}/>}
                {grid && (() => {
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
                                allCells.push(
                                    <CellEdges
                                        key={`cell-${x}-${y}-${z}`}
                                        center={[cx, cy, cz]}
                                        size={[cellLength, cellLength, layerHeight]}
                                    />
                                );
                            }
                        }
                    }
                    return <group>{allCells}</group>;
                })()}
                {startCell && (() => {
                    const pos = getCellCenterVector(startCell);
                    if (!pos) return null;
                    return (
                        <mesh position={pos}>
                            <sphereGeometry args={[0.3]}/>
                            <meshStandardMaterial color="green"/>
                        </mesh>
                    );
                })()}
                {endCell && (() => {
                    const pos = getCellCenterVector(endCell);
                    if (!pos) return null;
                    return (
                        <mesh position={pos}>
                            <sphereGeometry args={[0.3]}/>
                            <meshStandardMaterial color="blue"/>
                        </mesh>
                    );
                })()}
                {path && (
                    <group>
                        {path.map(([x, y, z], idx) => {
                            const pos = getCellCenterVector({getCoords: () => ({x, y, z})});
                            if (!pos) return null;
                            return (
                                <mesh key={idx} position={pos}>
                                    <sphereGeometry args={[0.2]}/>
                                    <meshStandardMaterial color="red"/>
                                </mesh>
                            );
                        })}
                    </group>
                )}
            </Canvas>
        </div>
    );
};

export default SceneView;
