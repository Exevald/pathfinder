import {
    obstaclesAtom,
    addObstacle,
    removeObstacle,
    updateObstacle,
    saveSandboxState,
    loadSandboxState
} from '../Model/atoms';
import {type Ctx, type Atom} from '@reatom/core';
import {Grid, PathPoint} from '../Model/Grid';
import {Cell} from '../Model/Cell';
import type {Obstacle} from '../Model/atoms';
import * as THREE from 'three';

export type SandboxEditMode = 'move' | 'rotate' | 'scale' | null;

export class SandboxViewModel {
    private readonly ctx: Ctx;
    private listeners: Set<() => void> = new Set();
    private isFlightMode: boolean = false;
    private selectedObstacleId: string | null = null;
    private editMode: SandboxEditMode = 'move';
    private pickMode: 'start' | 'end' | null = null;
    private readonly grid: Grid | null = null;
    private startCell: Cell | null = null;
    private endCell: Cell | null = null;
    private path: PathPoint[] | null = null;

    constructor(ctx: Ctx) {
        this.ctx = ctx;

        const atomsToWatch: Atom<any>[] = [obstaclesAtom];
        atomsToWatch.forEach(atom => {
            this.ctx.subscribe(atom, () => {
                this.recalculateGridCosts();
                this.notifyListeners();
            });
        });

        const GRID_SIZE_X = 20;
        const GRID_SIZE_Y = 20;
        const GRID_SIZE_Z = 3;
        const CELL_SIZE = 1;
        const offset: [number, number, number] = [-GRID_SIZE_X * CELL_SIZE / 2, 0, -GRID_SIZE_Y * CELL_SIZE / 2];
        this.grid = new Grid(GRID_SIZE_X, GRID_SIZE_Y, GRID_SIZE_Z, CELL_SIZE, CELL_SIZE, offset);
        this.recalculateGridCosts();
    }

    public get getObstacles(): Obstacle[] {
        return this.ctx.get(obstaclesAtom);
    }

    public get assertFlightMode(): boolean {
        return this.isFlightMode;
    }

    public get getSelectedObstacleId(): string | null {
        return this.selectedObstacleId;
    }

    public get getEditMode(): SandboxEditMode {
        return this.editMode;
    }

    public get getPickMode(): 'start' | 'end' | null {
        return this.pickMode;
    }

    public get getGrid(): Grid | null {
        return this.grid;
    }

    public get getStartCell(): Cell | null {
        return this.startCell;
    }

    public get getEndCell(): Cell | null {
        return this.endCell;
    }

    public get getPath(): PathPoint[] | null {
        return this.path;
    }

    public toggleFlightMode = (): void => {
        this.isFlightMode = !this.isFlightMode;
        if (this.isFlightMode) {
            this.selectObstacle(null);
        }
        this.notifyListeners();
    };

    public setEditMode = (mode: SandboxEditMode): void => {
        this.editMode = mode;
        this.notifyListeners();
    };

    public setPickMode = (mode: 'start' | 'end' | null): void => {
        this.pickMode = mode;
        this.notifyListeners();
    };

    public selectObstacle = (id: string | null): void => {
        this.selectedObstacleId = id;
        if (id && !this.editMode) {
            this.editMode = 'move';
        }
        if (!id) {
            this.editMode = null;
        }
        this.notifyListeners();
    };

    public addObstacle = (): void => {
        addObstacle(this.ctx, {
            position: [0, 0.5, 0],
            size: [1, 1, 1],
            color: '#' + Math.floor(Math.random() * 16777215).toString(16).padStart(6, '0')
        });
    };

    public removeSelectedObstacle = (): void => {
        if (this.selectedObstacleId) {
            removeObstacle(this.ctx, this.selectedObstacleId);
            this.selectObstacle(null);
        }
    };

    public updateObstacle = (id: string, updates: Partial<Omit<Obstacle, 'id'>>): void => {
        updateObstacle(this.ctx, id, updates);
    };

    public duplicateSelectedObstacle = (): void => {
        const obstacleToClone = this.getObstacles.find(o => o.id === this.selectedObstacleId);
        if (obstacleToClone) {
            this.addObstacleWithParams({
                ...obstacleToClone,
                position: [
                    obstacleToClone.position[0] + obstacleToClone.size[0] + 0.2,
                    obstacleToClone.position[1],
                    obstacleToClone.position[2]
                ],
            })
        }
    };

    public resetSelectedObstacle = (): void => {
        if (this.selectedObstacleId) {
            this.updateObstacle(this.selectedObstacleId, {position: [0, 0.5, 0], size: [1, 1, 1]});
        }
    };

    public saveState = (): void => {
        saveSandboxState(this.ctx);
    };

    public loadState = (file: File): void => {
        loadSandboxState(this.ctx, file);
    };

    private addObstacleWithParams = (params: Omit<Obstacle, 'id'>) => {
        addObstacle(this.ctx, params)
    }

    public handleGridClick = (point: THREE.Vector3): void => {
        if (!this.pickMode || !this.grid) return;

        const offset = this.grid.getOffset();
        const cellLength = this.grid.getCellLength();
        const layerHeight = this.grid.getLayerLength();
        const ix = Math.floor((point.x - offset[0]) / cellLength);
        const iy = Math.floor((point.y - offset[1]) / layerHeight);
        const iz = Math.floor((point.z - offset[2]) / cellLength);

        const cell = this.grid.getCellByIndices(ix, iy < 0 ? 0 : iy, iz);
        if (!cell) return;

        if (this.pickMode === 'start') {
            this.startCell = cell;
        } else {
            this.endCell = cell;
        }

        this.pickMode = null;
        this.notifyListeners();
    };

    public calculatePath = (): void => {
        if (this.grid && this.startCell && this.endCell) {
            this.path = this.grid.findPath(this.startCell, this.endCell);
            this.notifyListeners();
        }
    };

    public recalculateGridCosts = (): void => {
        if (!this.grid) return;

        const droneRadius = 0.3;
        const k = 1.5;
        const cellLength = this.grid.getCellLength();
        const layerHeight = this.grid.getLayerLength();
        const sizeX = this.grid.getGridSizeX();
        const sizeY = this.grid.getGridSizeY();
        const sizeZ = this.grid.getGridSizeZ();
        const cellCenter = new THREE.Vector3();
        const tempBox = new THREE.Box3();
        for (let z = 0; z < sizeZ; z++) {
            for (let y = 0; y < sizeY; y++) {
                for (let x = 0; x < sizeX; x++) {
                    const [cx, cy, cz] = this.grid.getCellCenter(x, y, z);
                    cellCenter.set(cx, cy, cz);
                    tempBox.setFromCenterAndSize(cellCenter, new THREE.Vector3(cellLength, cellLength, layerHeight));
                    let cost = 50;
                    let minDist = Infinity;
                    let intersects = false;
                    for (const obs of this.getObstacles) {
                        const [ox, oy, oz] = obs.position;
                        const [sx, sy, sz] = obs.size;
                        const obsBox = new THREE.Box3();
                        obsBox.setFromCenterAndSize(new THREE.Vector3(ox, oy, oz), new THREE.Vector3(sx, sy, sz));
                        if (obsBox.intersectsBox(tempBox)) {
                            intersects = true;
                            break;
                        } else {
                            const dist = obsBox.distanceToPoint(cellCenter);
                            if (dist < minDist) minDist = dist;
                        }
                    }
                    if (intersects) {
                        cost = 254;
                    } else if (minDist < droneRadius) {
                        cost = 253;
                    } else {
                        cost = Math.round(Math.exp(-k * (minDist - droneRadius)) * 203 + 50);
                    }
                    this.grid.setCellCost({x, y, z}, cost);
                }
            }
        }

        this.notifyListeners();
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