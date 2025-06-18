import {atom, action} from '@reatom/framework';
import {Grid} from './Grid';
import {Cell} from './Cell';
import * as THREE from 'three';
import {OBJLoader} from 'three/examples/jsm/loaders/OBJLoader';
import {MTLLoader} from 'three/examples/jsm/loaders/MTLLoader';
import {MeshBVH, acceleratedRaycast} from 'three-mesh-bvh';
import * as BufferGeometryUtils from 'three/examples/jsm/utils/BufferGeometryUtils.js';

(THREE.Mesh.prototype as any).raycast = acceleratedRaycast;

export const objFileAtom = atom<File | null>(null, 'objFile');
export const mtlFileAtom = atom<File | null>(null, 'mtlFile');
export const objObjectAtom = atom<THREE.Group | null>(null, 'objObject');
export const gridAtom = atom<Grid | null>(null, 'grid');
export const startCellAtom = atom<Cell | null>(null, 'startCell');
export const endCellAtom = atom<Cell | null>(null, 'endCell');
export const pathAtom = atom<[number, number, number, number][] | null>(null, 'path');
export const isLoadingAtom = atom<boolean>(false, 'isLoading');

export const setStartCell = action((ctx, cell: Cell) => startCellAtom(ctx, cell), 'setStartCell');
export const setEndCell = action((ctx, cell: Cell) => endCellAtom(ctx, cell), 'setEndCell');

export const calculatePath = action((ctx) => {
    const grid = ctx.get(gridAtom);
    const start = ctx.get(startCellAtom);
    const end = ctx.get(endCellAtom);
    if (grid && start && end) {
        const path = grid.findPath(start, end);
        pathAtom(ctx, path);
    }
}, 'calculatePath');

export const loadOBJMTL = action((ctx, {objFile, mtlFile}: { objFile: File; mtlFile: File }) => {
    isLoadingAtom(ctx, true);
    gridAtom(ctx, null);
    pathAtom(ctx, null);
    startCellAtom(ctx, null);
    endCellAtom(ctx, null);

    const mtlReader = new FileReader();
    mtlReader.onload = () => {
        const mtlContent = mtlReader.result as string;
        const mtlLoader = new MTLLoader();
        const materials = mtlLoader.parse(mtlContent, '');
        materials.preload();

        const objReader = new FileReader();
        objReader.onload = () => {
            const objContent = objReader.result as string;
            const objLoader = new OBJLoader();
            objLoader.setMaterials(materials);
            const obj = objLoader.parse(objContent);

            const bbox = new THREE.Box3().setFromObject(obj);
            const center = bbox.getCenter(new THREE.Vector3());
            obj.position.set(-center.x, -center.y, -center.z);
            obj.updateMatrixWorld(true);

            objObjectAtom(ctx, obj);

            setTimeout(() => {
                const cellLength = 0.5;
                const layerHeight = 0.5;
                const droneRadius = 0.1;
                const k = 0.5;

                const geometries: THREE.BufferGeometry[] = [];
                obj.traverse((child) => {
                    if ((child as THREE.Mesh).isMesh) {
                        const clonedGeom = (child as THREE.Mesh).geometry.clone();
                        clonedGeom.applyMatrix4(child.matrixWorld);
                        geometries.push(clonedGeom);
                    }
                });
                const staticGeometry = BufferGeometryUtils.mergeGeometries(geometries);
                const bvh = new MeshBVH(staticGeometry);

                const sizeX = Math.ceil((bbox.max.x - bbox.min.x) / cellLength);
                const sizeY = Math.ceil((bbox.max.y - bbox.min.y) / cellLength);
                const sizeZ = Math.ceil((bbox.max.z - bbox.min.z) / layerHeight);
                const offset: [number, number, number] = [
                    -((bbox.max.x - bbox.min.x) / 2),
                    -((bbox.max.y - bbox.min.y) / 2),
                    -((bbox.max.z - bbox.min.z) / 2)
                ];
                const grid = new Grid(sizeX, sizeY, sizeZ, layerHeight, cellLength, offset);

                const tempBox = new THREE.Box3();
                const cellCenter = new THREE.Vector3();
                const identityMatrix = new THREE.Matrix4();
                const targetInfo = {
                    point: new THREE.Vector3(),
                    distance: 0,
                    faceIndex: -1
                };

                for (let z = 0; z < sizeZ; z++) {
                    for (let y = 0; y < sizeY; y++) {
                        for (let x = 0; x < sizeX; x++) {
                            const [cx, cy, cz] = grid.getCellCenter(x, y, z);
                            cellCenter.set(cx, cy, cz);
                            tempBox.setFromCenterAndSize(cellCenter, new THREE.Vector3(cellLength, cellLength, layerHeight));

                            let cost = 50;
                            if (bvh.intersectsBox(tempBox, identityMatrix)) {
                                cost = 254;
                            } else {
                                bvh.closestPointToPoint(cellCenter, targetInfo);
                                const distance = cellCenter.distanceTo(targetInfo.point);
                                if (distance < droneRadius) {
                                    cost = 253;
                                } else {
                                    cost = Math.round(Math.exp(-k * (distance - droneRadius)) * 203 + 50);
                                }
                            }
                            grid.setCellCost({x, y, z}, cost);
                        }
                    }
                }

                gridAtom(ctx, grid);
                isLoadingAtom(ctx, false);
            }, 10);
        };
        objReader.readAsText(objFile);
    };
    mtlReader.readAsText(mtlFile);
});

export type Mode = 'obj' | 'sandbox';

export interface Obstacle {
    id: string;
    position: [number, number, number];
    size: [number, number, number];
    color: string;
}

export const modeAtom = atom<Mode>('obj', 'mode');
export const obstaclesAtom = atom<Obstacle[]>([], 'obstacles');

export const addObstacle = action((ctx, obstacle: Omit<Obstacle, 'id'>) => {
    const obstacles = ctx.get(obstaclesAtom);
    const newObstacle = {
        ...obstacle,
        id: Math.random().toString(36).substr(2, 9)
    };
    obstaclesAtom(ctx, [...obstacles, newObstacle]);
});

export const removeObstacle = action((ctx, id: string) => {
    const obstacles = ctx.get(obstaclesAtom);
    obstaclesAtom(ctx, obstacles.filter(o => o.id !== id));
});

export const updateObstacle = action((ctx, id: string, updates: Partial<Omit<Obstacle, 'id'>>) => {
    const obstacles = ctx.get(obstaclesAtom);
    obstaclesAtom(ctx, obstacles.map(o =>
        o.id === id ? {...o, ...updates} : o
    ));
});

export const setMode = action((ctx, mode: Mode) => {
    modeAtom(ctx, mode);
});

export const saveSandboxState = action((ctx) => {
    const obstacles = ctx.get(obstaclesAtom);
    const state = {
        obstacles,
        version: '1.0'
    };
    const blob = new Blob([JSON.stringify(state, null, 2)], {type: 'application/json'});
    const url = URL.createObjectURL(blob);
    const a = document.createElement('a');
    a.href = url;
    a.download = 'sandbox-state.json';
    document.body.appendChild(a);
    a.click();
    document.body.removeChild(a);
    URL.revokeObjectURL(url);
});

export const loadSandboxState = action((ctx, file: File) => {
    const reader = new FileReader();
    reader.onload = (e) => {
        const state = JSON.parse(e.target?.result as string);
        if (state.version === '1.0' && Array.isArray(state.obstacles)) {
            obstaclesAtom(ctx, state.obstacles);
        }
    };
    reader.readAsText(file);
});