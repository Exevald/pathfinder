import React, {useEffect, useMemo, useRef, useState} from 'react';
import {Canvas} from '@react-three/fiber';
import {PointerLockControls} from '@react-three/drei';
import * as THREE from 'three';
import {PathfindingViewModel} from '../ViewModel/PathfindingViewModel';
import {useCtx} from '@reatom/npm-react';
import {FPSMovement} from "./components/FPSMovement";
import {GridDisplay} from "./components/GridDisplay";
import {CameraControls} from "./components/CameraControls";
import {ThreeEvent} from "@react-three/fiber/dist/declarations/src/core/events";

const SceneView: React.FC = () => {
    const ctx = useCtx();
    const viewModel = useMemo(() => new PathfindingViewModel(ctx), [ctx]);
    const [, forceUpdate] = useState(0);

    useEffect(() => {
        return viewModel.subscribe(() => {
            forceUpdate(c => c + 1);
        });
    }, [viewModel]);

    const [controlsEnabled, setControlsEnabled] = useState(false);
    const [pickMode, setPickMode] = useState<'start' | 'end' | null>(null);
    const canvasRef = useRef<HTMLDivElement>(null);
    const [showGrid, setShowGrid] = useState(true);
    const [showCostText, setShowCostText] = useState(false);

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

    const handleSceneClick = (e: ThreeEvent<MouseEvent>) => {
        if (!pickMode) return;
        const point = e.point as THREE.Vector3;
        if (!point) return;

        const cell = viewModel.getCellByPosition(point);
        if (!cell) return;

        if (pickMode === 'start') viewModel.setStart(cell);
        if (pickMode === 'end') viewModel.setEnd(cell);

        setPickMode(null);
    };

    const {getObjObject, getStartCell, getEndCell, getPath, getGrid} = viewModel;

    return (
        <div style={{position: 'relative', width: '100%', height: '100%'}} ref={canvasRef}>
            <div style={{position: 'absolute', zIndex: 2, left: 20, top: 20}}>
                <button onClick={() => setPickMode('start')}>Выбрать старт</button>
                <button onClick={() => setPickMode('end')}>Выбрать финиш</button>
                <button onClick={viewModel.calculatePath}
                        disabled={!viewModel.getStartCell || !viewModel.getEndCell}>Построить
                    путь
                </button>
                <button onClick={() => setShowGrid(v => !v)}>{showGrid ? 'Скрыть сетку' : 'Показать сетку'}</button>
                <button onClick={() => setShowCostText(v => !v)}>
                    {showCostText ? 'Скрыть стоимость' : 'Показать стоимость'}
                </button>
            </div>

            <CameraControls controlsEnabled={controlsEnabled} onEnter3D={() => setControlsEnabled(true)}/>
            <Canvas camera={{position: [10, 10, 10], fov: 60}} style={{width: '100%', height: '100%'}}>
                <ambientLight intensity={0.7}/>
                {controlsEnabled && <PointerLockControls/>}
                <FPSMovement enabled={controlsEnabled}/>
                {pickMode && (
                    <group>
                        <mesh position={[0, 0, 0]} rotation={[-Math.PI / 2, 0, 0]} onClick={handleSceneClick}>
                            <planeGeometry args={[1000, 1000]}/>
                            <meshBasicMaterial transparent opacity={0}/>
                        </mesh>
                        <mesh position={[0, 0, 0]} rotation={[0, 0, 0]} onClick={handleSceneClick}>
                            <planeGeometry args={[1000, 1000]}/>
                            <meshBasicMaterial transparent opacity={0}/>
                        </mesh>
                        <mesh position={[0, 0, 0]} rotation={[0, Math.PI / 2, 0]} onClick={handleSceneClick}>
                            <planeGeometry args={[1000, 1000]}/>
                            <meshBasicMaterial transparent opacity={0}/>
                        </mesh>
                    </group>
                )}
                {getObjObject && <primitive object={getObjObject}/>}
                {viewModel.getGrid && showGrid &&
                    <GridDisplay grid={viewModel.getGrid} showCostText={showCostText} isSandbox={false}/>}
                {getStartCell && (() => {
                    const pos = viewModel.getCellCenterVector(getStartCell);
                    if (!pos) return null;
                    return (
                        <mesh position={[pos.x, pos.z, pos.y]}>
                            <sphereGeometry args={[0.15]}/>
                            <meshStandardMaterial color="green"/>
                        </mesh>
                    );
                })()}
                {getEndCell && (() => {
                    const pos = viewModel.getCellCenterVector(getEndCell);
                    if (!pos) return null;
                    return (
                        <mesh position={[pos.x, pos.z, pos.y]}>
                            <sphereGeometry args={[0.15]}/>
                            <meshStandardMaterial color="blue"/>
                        </mesh>
                    );
                })()}
                {getPath && getGrid && (
                    <group>
                        {getPath.map(([x, y, z], idx) => {
                            const [cx, cy, cz] = getGrid.getCellCenter(x, y, z);
                            return (
                                <mesh key={idx} position={[cx, cy, cz]}>
                                    <sphereGeometry args={[0.03]}/>
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