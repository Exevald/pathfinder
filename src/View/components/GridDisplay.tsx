import React from 'react';
import * as THREE from 'three';
import {Text} from '@react-three/drei';
import {Grid} from '../../Model/Grid';

function getBoxEdges(center: [number, number, number], size: [number, number, number]) {
    const [cx, cy, cz] = center;
    const [sx, sy, sz] = size;
    const hx = sx / 2, hy = sy / 2, hz = sz / 2;
    const v = [
        [cx - hx, cy - hy, cz - hz], [cx + hx, cy - hy, cz - hz],
        [cx + hx, cy + hy, cz - hz], [cx - hx, cy + hy, cz - hz],
        [cx - hx, cy - hy, cz + hz], [cx + hx, cy - hy, cz + hz],
        [cx + hx, cy + hy, cz + hz], [cx - hx, cy + hy, cz + hz],
    ];
    const edges = [
        [v[0], v[1]], [v[1], v[2]], [v[2], v[3]], [v[3], v[0]],
        [v[4], v[5]], [v[5], v[6]], [v[6], v[7]], [v[7], v[4]],
        [v[0], v[4]], [v[1], v[5]], [v[2], v[6]], [v[3], v[7]],
    ];
    return edges.flat() as [number, number, number][];
}

const CellEdges: React.FC<{
    center: [number, number, number];
    size: [number, number, number];
    cost: number;
    showText: boolean;
}> = ({center, size, cost, showText}) => {
    const points = getBoxEdges(center, size).map(([x, y, z]) => new THREE.Vector3(x, y, z));
    return (
        <group>
            <lineSegments>
                <bufferGeometry>
                    <bufferAttribute
                        attach="attributes-position"
                        args={[new Float32Array(points.flatMap(p => [p.x, p.y, p.z])), 3]}
                    />
                </bufferGeometry>
                <lineBasicMaterial color="red" linewidth={1.5}/>
            </lineSegments>
            {showText && (
                <Text position={center} fontSize={0.05} color="white" anchorX="center" anchorY="middle">
                    {cost.toFixed(1)}
                </Text>
            )}
        </group>
    );
};

interface GridDisplayProps {
    grid: Grid;
    showCostText: boolean;
    isSandbox: boolean;
}

export const GridDisplay: React.FC<GridDisplayProps> = ({grid, showCostText, isSandbox}) => {
    const cellLength = grid.getCellLength();
    const layerHeight = grid.getLayerLength();
    const gridSizeX = grid.getGridSizeX();
    const gridSizeY = grid.getGridSizeY();
    const gridSizeZ = grid.getGridSizeZ();
    const offset = grid.getOffset ? grid.getOffset() : [0, 0, 0];

    const allCells = React.useMemo(() => {
        const cells = [];
        for (let x = 0; x < gridSizeX; ++x) {
            for (let y = 0; y < gridSizeY; ++y) {
                for (let z = 0; z < gridSizeZ; ++z) {
                    const cx = offset[0] + (x + 0.5) * cellLength;
                    let cy = offset[1] + (y + 0.5) * cellLength;
                    let cz = offset[2] + (z + 0.5) * layerHeight;
                    if (isSandbox) {
                        cy = offset[1] + (z + 0.5) * layerHeight;
                        cz = offset[2] + (y + 0.5) * cellLength;
                    }
                    const cell = grid.getCellByIndices(x, y, z);
                    if (cell) {
                        cells.push(
                            <CellEdges
                                key={`cell-${x}-${y}-${z}`}
                                center={[cx, cy, cz]}
                                size={[cellLength, cellLength, layerHeight]}
                                cost={cell.getCost()}
                                showText={showCostText}
                            />
                        );
                    }
                }
            }
        }
        return cells;
    }, [gridSizeX, gridSizeY, gridSizeZ, offset, cellLength, layerHeight, isSandbox, grid, showCostText]);


    return (
        <group>
            <axesHelper args={[Math.max(gridSizeX, gridSizeY, gridSizeZ) * cellLength * 2]}
                        position={[offset[0], offset[1], offset[2]]}/>
            {allCells}
        </group>
    );
};