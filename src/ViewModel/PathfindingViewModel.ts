import {
    objObjectAtom,
    gridAtom,
    startCellAtom,
    endCellAtom,
    pathAtom,
    objFileAtom,
    mtlFileAtom,
    setStartCell,
    setEndCell,
    calculatePath,
    loadOBJMTL
} from '../Model/atoms';
import {type Atom, type Ctx} from '@reatom/core';
import * as THREE from 'three';
import {Cell} from '../Model/Cell';
import {Grid} from '../Model/Grid';

export class PathfindingViewModel {
    private readonly ctx: Ctx;
    private listeners: Set<() => void> = new Set();

    constructor(ctx: Ctx) {
        this.ctx = ctx;

        const atomsToWatch: Atom<any>[] = [
            objObjectAtom,
            gridAtom,
            startCellAtom,
            endCellAtom,
            pathAtom,
            objFileAtom,
            mtlFileAtom
        ];

        atomsToWatch.forEach(atom => {
            this.ctx.subscribe(atom, () => this.notifyListeners());
        });
    }

    public get getObjObject(): THREE.Group | null {
        return this.ctx.get(objObjectAtom);
    }

    public get getGrid(): Grid | null {
        return this.ctx.get(gridAtom);
    }

    public get getStartCell(): Cell | null {
        return this.ctx.get(startCellAtom);
    }

    public get getEndCell(): Cell | null {
        return this.ctx.get(endCellAtom);
    }

    public get getPath(): [number, number, number, number][] | null {
        return this.ctx.get(pathAtom);
    }

    public get getObjFile(): File | null {
        return this.ctx.get(objFileAtom);
    }

    public get getMtlFile(): File | null {
        return this.ctx.get(mtlFileAtom);
    }

    public setObjFile = (file: File): void => {
        objFileAtom(this.ctx, file);
    };

    public setMtlFile = (file: File): void => {
        mtlFileAtom(this.ctx, file);
    };

    public setStart = (cell: Cell): void => {
        setStartCell(this.ctx, cell);
    };

    public setEnd = (cell: Cell): void => {
        setEndCell(this.ctx, cell);
    };

    public calculatePath = (): void => {
        if (this.getStartCell && this.getEndCell) {
            calculatePath(this.ctx);
        }
    };

    public loadObjMtl = (objFile: File, mtlFile: File): void => {
        loadOBJMTL(this.ctx, {objFile, mtlFile});
    };

    public getCellByPosition = (pos: THREE.Vector3): Cell | null => {
        const grid = this.getGrid;
        if (!grid) return null;

        const offset = grid.getOffset();
        const cellLength = grid.getCellLength();
        const layerHeight = grid.getLayerLength();

        const ix = Math.floor((pos.x - offset[0]) / cellLength);
        const iy = Math.floor((pos.y - offset[1]) / cellLength);
        const iz = Math.floor((pos.z - offset[2]) / layerHeight);

        return grid.getCellByIndices(ix, iy, iz);
    };

    public getCellCenterVector = (cell: Cell): THREE.Vector3 | null => {
        const grid = this.getGrid;
        if (!grid || !cell) return null;
        const {x, y, z} = cell.getCoords();
        const [cx, cy, cz] = grid.getCellCenter(x, y, z);
        return new THREE.Vector3(cx, cz, cy);
    };

    public subscribe = (listener: () => void): (() => void) => {
        this.listeners.add(listener);
        return () => {
            this.listeners.delete(listener);
        };
    };

    private notifyListeners = (): void => {
        this.listeners.forEach(listener => listener());
    };
}