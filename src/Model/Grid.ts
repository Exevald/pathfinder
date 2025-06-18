import {Cell} from './Cell';
import {Coords} from './Coords';
import {BezierCurve} from "./BezierCurve";

export type PathPoint = [number, number, number, number];

function evaluateSegmentSafety(segment: Cell[]): number {
    let totalCost = 0;
    for (const cell of segment) {
        totalCost += cell.getCost();
    }
    return totalCost / segment.length;
}

function calculateSegmentTime(safetyScore: number): number {
    const baseTime = 1.0;
    const slowdownFactor = 2.0;
    return baseTime + (safetyScore - 50.0) / 254.0 * slowdownFactor;
}

function smoothPath(path: Cell[], cellHeight: number): PathPoint[] {
    if (path.length === 0) return [];
    const waypoints: [number, number, number][] = path.map(cell => {
        const coords = cell.getCoords();
        const cellLength = cell.getCellLength();
        return [
            coords.x * cellLength,
            coords.y * cellLength,
            coords.z * cellHeight
        ];
    });
    if (waypoints.length < 2) return [];

    const curve = new BezierCurve(waypoints);
    const segmentsCount = 100;
    let totalTime = 0.0;
    const smoothPathWithSpeed: PathPoint[] = [];

    for (let i = 0; i <= segmentsCount; ++i) {
        let t = i / segmentsCount;
        if (i === segmentsCount) t = 1.0;
        const point = curve.evaluate(t);

        const segmentIndex = Math.min(Math.floor(i * (path.length - 1) / segmentsCount), path.length - 2);
        const safetyScore = evaluateSegmentSafety([path[segmentIndex], path[segmentIndex + 1]]);
        const segmentTime = calculateSegmentTime(safetyScore);

        totalTime += segmentTime / segmentsCount;
        smoothPathWithSpeed.push([point[0], point[1], point[2], totalTime]);
    }
    return smoothPathWithSpeed;
}

export class Grid {
    private readonly gridSizeX: number;
    private readonly gridSizeY: number;
    private readonly gridSizeZ: number;
    private readonly layerHeight: number;
    private readonly cellLength: number;
    private readonly cells: Cell[][][];
    private readonly offset: [number, number, number];

    constructor(
        sizeX: number,
        sizeY: number,
        sizeZ: number,
        layerHeight: number,
        cellLength: number,
        offset: [number, number, number] = [0, 0, 0]
    ) {
        this.gridSizeX = sizeX;
        this.gridSizeY = sizeY;
        this.gridSizeZ = sizeZ;
        this.layerHeight = layerHeight;
        this.cellLength = cellLength;
        this.offset = offset;

        this.cells = Array.from({length: sizeX}, (_, x) =>
            Array.from({length: sizeY}, (_, y) =>
                Array.from({length: sizeZ}, (_, z) => new Cell({x, y, z}, cellLength))
            )
        );
    }

    public setCellCost(coords: Coords, cost: number): void {
        const cell = this.getCellByIndices(coords.x, coords.y, coords.z);
        if (cell) {
            cell.setCost(cost);
        }
    }

    public findPath(start: Cell, goal: Cell): PathPoint[] {
        if (!this.isCellInGrid(start) || !this.isCellInGrid(goal)) {
            return [];
        }

        type NodeWithPriority = { cell: Cell; priority: number };
        const openSet: NodeWithPriority[] = [];
        const cameFrom = new Map<Cell, Cell | undefined>();
        const gScore = new Map<Cell, number>();

        this.cells.flat(2).forEach(cell => gScore.set(cell, Infinity));
        gScore.set(start, 0);

        openSet.push({cell: start, priority: this.heuristic(start, goal)});

        while (openSet.length > 0) {
            openSet.sort((a, b) => a.priority - b.priority);
            const current = openSet.shift()!.cell;

            if (current === goal) {
                const path: Cell[] = [];
                let currentPtr: Cell | undefined = current;
                while (currentPtr) {
                    path.push(currentPtr);
                    currentPtr = cameFrom.get(currentPtr);
                }
                path.reverse();
                return smoothPath(path, this.layerHeight);
            }

            for (const neighbor of this.getValidNeighbors(current)) {
                const moveCost = (current.getCost() + neighbor.getCost()) / 2;
                const tentativeG = (gScore.get(current) ?? Infinity) + moveCost;

                if (tentativeG < (gScore.get(neighbor) ?? Infinity)) {
                    cameFrom.set(neighbor, current);
                    gScore.set(neighbor, tentativeG);
                    const f = tentativeG + this.heuristic(neighbor, goal);
                    if (!openSet.some(node => node.cell === neighbor)) {
                        openSet.push({cell: neighbor, priority: f});
                    }
                }
            }
        }
        return [];
    }

    public getCells = (): Cell[][][] => this.cells;
    public getCellLength = (): number => this.cellLength;
    public getLayerLength = (): number => this.layerHeight;
    public getGridSizeX = (): number => this.gridSizeX;
    public getGridSizeY = (): number => this.gridSizeY;
    public getGridSizeZ = (): number => this.gridSizeZ;
    public getOffset = (): [number, number, number] => this.offset;

    public getCellByIndices(x: number, y: number, z: number): Cell | null {
        if (x >= 0 && x < this.gridSizeX && y >= 0 && y < this.gridSizeY && z >= 0 && z < this.gridSizeZ) {
            return this.cells[x][y][z];
        }
        return null;
    }

    public getCellCenter(x: number, y: number, z: number): [number, number, number] {
        const cx = this.offset[0] + x * this.cellLength;
        const cy = this.offset[1] + y * this.cellLength;
        const cz = this.offset[2] + z * this.layerHeight;
        return [cx, cy, cz];
    }

    private isCellInGrid(cell: Cell): boolean {
        if (!cell) return false;
        const {x, y, z} = cell.getCoords();
        return x >= 0 && x < this.gridSizeX && y >= 0 && y < this.gridSizeY && z >= 0 && z < this.gridSizeZ;
    }

    private getValidNeighbors(cell: Cell): Cell[] {
        const neighbors: Cell[] = [];
        const {x, y, z} = cell.getCoords();
        for (let dz = -1; dz <= 1; dz++) {
            for (let dy = -1; dy <= 1; dy++) {
                for (let dx = -1; dx <= 1; dx++) {
                    if (dx === 0 && dy === 0 && dz === 0) continue;
                    const neighbor = this.getCellByIndices(x + dx, y + dy, z + dz);
                    if (neighbor && neighbor.getCost() < 254) {
                        neighbors.push(neighbor);
                    }
                }
            }
        }
        return neighbors;
    }

    private heuristic(a: Cell, b: Cell): number {
        const aC = a.getCoords();
        const bC = b.getCoords();
        const dx = (aC.x - bC.x) * this.cellLength;
        const dy = (aC.y - bC.y) * this.cellLength;
        const dz = (aC.z - bC.z) * this.layerHeight;
        const verticalPenaltyFactor = 2.0;
        return Math.sqrt(dx ** 2 + dy ** 2 + (dz * verticalPenaltyFactor) ** 2);
    }
}