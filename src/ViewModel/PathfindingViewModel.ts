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
            return null
        }
        const ix = Math.floor(pos.x / grid.getCellLength());
        const iy = Math.floor(pos.y / grid.getCellLength());
        const iz = Math.floor(pos.z / grid.getCellLength());
        return grid.getCellByIndices(ix, iy, iz);
    };

    const getCellCenterVector = (cell: any) => {
        if (!grid || !cell) return null;
        const {x, y, z} = cell.getCoords();
        const [cx, cy, cz] = grid.getCellCenter(x, y, z);
        return new THREE.Vector3(cx, cy, cz);
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
