import {useAtom} from '@reatom/npm-react';
import {
    objFileAtom, mtlFileAtom, objObjectAtom, gridAtom,
    startCellAtom, endCellAtom, pathAtom,
    setStartCell, setEndCell, calculatePath, loadOBJMTL
} from '../Model/atoms';
import {useAction} from '@reatom/npm-react';
import * as THREE from 'three';

export const usePathfindingVM = () => {
    const [objFile, setObjFile] = useAtom(objFileAtom);
    const [mtlFile, setMtlFile] = useAtom(mtlFileAtom);
    const [objObject] = useAtom(objObjectAtom);
    const [grid] = useAtom(gridAtom);
    const [startCell] = useAtom(startCellAtom);
    const [endCell] = useAtom(endCellAtom);
    const [path] = useAtom(pathAtom);

    const setStart = useAction(setStartCell);
    const setEnd = useAction(setEndCell);
    const calcPath = useAction(calculatePath);
    const loadObjMtl = useAction(loadOBJMTL);

    const getCellByPosition = (pos: THREE.Vector3) => {
        if (!grid) {
            return null;
        }
        const offset = grid.getOffset ? grid.getOffset() : [0, 0, 0];
        const sizeX = grid.getGridSizeX();
        const sizeY = grid.getGridSizeY();
        const sizeZ = grid.getGridSizeZ();
        const cellLength = grid.getCellLength();
        const layerHeight = grid.getLayerLength();

        if (
            pos.x < offset[0] || pos.x > offset[0] + sizeX * cellLength ||
            pos.y < offset[1] || pos.y > offset[1] + sizeY * cellLength ||
            pos.z < offset[2] || pos.z > offset[2] + sizeZ * layerHeight
        ) {
            return null;
        }
        const ix = Math.floor((pos.x - offset[0]) / cellLength);
        const iy = Math.floor((pos.y - offset[1]) / cellLength);
        const iz = Math.floor((pos.z - offset[2]) / layerHeight);
        if (
            ix < 0 || ix >= sizeX ||
            iy < 0 || iy >= sizeY ||
            iz < 0 || iz >= sizeZ
        ) {
            return null;
        }
        return grid.getCellByIndices(ix, iy, iz);
    };

    const getCellCenterVector = (cell: any) => {
        if (!grid || !cell) return null;
        const {x, y, z} = cell.getCoords();
        const [cx, cy, cz] = grid.getCellCenter(x, y, z);
        return new THREE.Vector3(cx, cz, cy);
    };

    return {
        objFile, setObjFile,
        mtlFile, setMtlFile,
        objObject,
        grid,
        startCell,
        endCell,
        path,
        setStart,
        setEnd,
        calcPath,
        loadObjMtl,
        getCellByPosition,
        getCellCenterVector,
    };
};
