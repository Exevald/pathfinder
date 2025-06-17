import {useAtom, useAction} from '@reatom/npm-react';
import {
    objFileAtom, mtlFileAtom, objObjectAtom, gridAtom,
    startCellAtom, endCellAtom, pathAtom, isLoadingAtom,
    setStartCell, setEndCell, calculatePath, loadOBJMTL
} from '../Model/atoms';
import * as THREE from 'three';
import {Cell} from '../Model/Cell';

export const usePathfindingVM = () => {
    const [objFile, setObjFile] = useAtom(objFileAtom);
    const [mtlFile, setMtlFile] = useAtom(mtlFileAtom);
    const [objObject] = useAtom(objObjectAtom);
    const [grid] = useAtom(gridAtom);
    const [startCell] = useAtom(startCellAtom);
    const [endCell] = useAtom(endCellAtom);
    const [path] = useAtom(pathAtom);
    const [isLoading] = useAtom(isLoadingAtom);

    const setStart = useAction(setStartCell);
    const setEnd = useAction(setEndCell);
    const calcPath = useAction(calculatePath);
    const loadObjMtl = useAction((ctx, objFile: File, mtlFile: File) => {
        loadOBJMTL(ctx, {objFile, mtlFile});
    });

    const getCellByPosition = (pos: THREE.Vector3): Cell | null => {
        if (!grid) return null;

        const offset = grid.getOffset();
        const cellLength = grid.getCellLength();
        const layerHeight = grid.getLayerLength();

        const ix = Math.floor((pos.x - offset[0]) / cellLength);
        const iy = Math.floor((pos.y - offset[1]) / cellLength);
        const iz = Math.floor((pos.z - offset[2]) / layerHeight);

        return grid.getCellByIndices(ix, iy, iz);
    };

    const getCellCenterVector = (cell: Cell): THREE.Vector3 | null => {
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
        isLoading,
        setStart,
        setEnd,
        calcPath,
        loadObjMtl,
        getCellByPosition,
        getCellCenterVector,
    };
};