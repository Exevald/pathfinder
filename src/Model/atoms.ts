import {atom, action} from '@reatom/framework';
import {Grid} from './Grid';
import {Cell} from './Cell';
import * as THREE from 'three';
import {OBJLoader} from 'three/examples/jsm/loaders/OBJLoader';
import {MTLLoader} from 'three/examples/jsm/loaders/MTLLoader';
import {Obstacle} from './Obstacle';

export const objFileAtom = atom<File | null>(null, 'objFile');
export const mtlFileAtom = atom<File | null>(null, 'mtlFile');
export const objObjectAtom = atom<THREE.Group | null>(null, 'objObject');
export const gridAtom = atom<Grid | null>(null, 'grid');
export const startCellAtom = atom<Cell | null>(null, 'startCell');
export const endCellAtom = atom<Cell | null>(null, 'endCell');
export const pathAtom = atom<[number, number, number, number][] | null>(null, 'path');
export const rawPathAtom = atom<[number, number, number][] | null>(null, 'rawPath');

export const setStartCell = action((ctx, cell: Cell) => {
    startCellAtom(ctx, cell);
}, 'setStartCell');

export const setEndCell = action((ctx, cell: Cell) => {
    endCellAtom(ctx, cell);
}, 'setEndCell');

export const calculatePath = action((ctx) => {
    const grid = ctx.get(gridAtom);
    const start = ctx.get(startCellAtom);
    const end = ctx.get(endCellAtom);
    if (grid && start && end) {
        const rawPathCells = grid.findRawPath(start, end);
        if (rawPathCells) {
            const rawPath = rawPathCells.map((cell: any) => {
                const {x, y, z} = cell.getCoords();
                return [x, y, z] as [number, number, number];
            });
            rawPathAtom(ctx, rawPath as [number, number, number][]);
        } else {
            rawPathAtom(ctx, null);
        }
        const path = grid.findPath(start, end);
        pathAtom(ctx, path);
    }
}, 'calculatePath');

export const loadOBJMTL = action((ctx, objFile: File, mtlFile: File) => {
    const mtlReader = new FileReader();
    mtlReader.onload = () => {
        const mtlContent = mtlReader.result as string;
        const mtlLoader = new MTLLoader();
        const mtlUrl = URL.createObjectURL(mtlFile);
        const mtlPath = mtlUrl.substring(0, mtlUrl.lastIndexOf('/') + 1);
        const materials = mtlLoader.parse(mtlContent, mtlPath);
        materials.preload();

        const objReader = new FileReader();
        objReader.onload = () => {
            const objContent = objReader.result as string;
            const objLoader = new OBJLoader();
            objLoader.setMaterials(materials);
            const obj = objLoader.parse(objContent);
            objObjectAtom(ctx, obj);

            const bbox = new THREE.Box3().setFromObject(obj);
            obj.traverse(child => {
                if ((child as THREE.Mesh).isMesh) {
                    bbox.expandByObject(child);
                }
            });
            const center = bbox.getCenter(new THREE.Vector3());
            obj.position.set(-center.x, -center.y, -center.z);

            const cellLength = 1;
            const layerHeight = 1;
            const droneRadius = 1;
            const k = 1;
            const sizeX = Math.ceil((bbox.max.x - bbox.min.x) / cellLength);
            const sizeY = Math.ceil((bbox.max.y - bbox.min.y) / cellLength);
            const sizeZ = Math.ceil((bbox.max.z - bbox.min.z) / layerHeight);
            const offset: [number, number, number] = [
                -((bbox.max.x - bbox.min.x) / 2),
                -((bbox.max.y - bbox.min.y) / 2),
                -((bbox.max.z - bbox.min.z) / 2)
            ];
            const grid = new Grid(sizeX, sizeY, sizeZ, layerHeight, droneRadius, k, cellLength, offset);

            obj.traverse(child => {
                if ((child as THREE.Mesh).isMesh) {
                    const mesh = child as THREE.Mesh;
                    const meshBbox = new THREE.Box3().setFromObject(mesh);
                    const minX = Math.floor((meshBbox.min.x - offset[0]) / cellLength);
                    const maxX = Math.ceil((meshBbox.max.x - offset[0]) / cellLength) - 1;
                    const minY = Math.floor((meshBbox.min.y - offset[1]) / cellLength);
                    const maxY = Math.ceil((meshBbox.max.y - offset[1]) / cellLength) - 1;
                    const minZ = Math.floor((meshBbox.min.z - offset[2]) / layerHeight);
                    const maxZ = Math.ceil((meshBbox.max.z - offset[2]) / layerHeight) - 1;
                    grid.addObstacle(
                        new Obstacle(
                            minX, maxX, minY, maxY, minZ, maxZ
                        )
                    );
                }
            });

            gridAtom(ctx, grid);
            startCellAtom(ctx, null);
            endCellAtom(ctx, null);
            pathAtom(ctx, null);
        };
        objReader.readAsText(objFile);
    };
    mtlReader.readAsText(mtlFile);
}, 'loadOBJMTL');
