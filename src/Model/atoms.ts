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
        console.time('Pathfinding');
        const path = grid.findPath(start, end);
        console.log(path)
        console.timeEnd('Pathfinding');
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
                console.time('Grid and Cost Calculation');

                const cellLength = 1;
                const layerHeight = 1;
                const droneRadius = 1.0;
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

                const modelSize = bbox.getSize(new THREE.Vector3());
                const sizeX = Math.ceil(modelSize.x / cellLength);
                const sizeY = Math.ceil(modelSize.z / cellLength);
                const sizeZ = Math.ceil(modelSize.y / layerHeight);
                const offset: [number, number, number] = [
                    bbox.min.x - center.x,
                    bbox.min.z - center.z,
                    bbox.min.y - center.y
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

                console.timeEnd('Grid and Cost Calculation');

                gridAtom(ctx, grid);
                isLoadingAtom(ctx, false);
            }, 10);
        };
        objReader.readAsText(objFile);
    };
    mtlReader.readAsText(mtlFile);
});