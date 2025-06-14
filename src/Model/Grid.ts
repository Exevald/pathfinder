import {Cell} from './Cell';
import {Coords} from './Coords';
import {Obstacle} from './Obstacle';
import {BezierCurve} from './BezierCurve';

type PathPoint = [number, number, number, number];

function isVerticalTransitionAllowed(coords: Coords): boolean {
    return coords.x % 5 === 0 && coords.y % 5 === 0;
}

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
    private readonly droneRadius: number;
    private readonly k: number;
    private readonly cellLength: number;
    private readonly cells: Cell[][][];
    private readonly obstacles: Obstacle[];
    private readonly offset: [number, number, number];

    constructor(sizeX: number, sizeY: number, sizeZ: number, layerHeight: number, droneRadius: number, k: number, cellLength: number, offset: [number, number, number] = [0, 0, 0]) {
        this.gridSizeX = sizeX;
        this.gridSizeY = sizeY;
        this.gridSizeZ = sizeZ;
        this.layerHeight = layerHeight;
        this.droneRadius = droneRadius;
        this.k = k;
        this.cellLength = cellLength;
        this.offset = offset;
        this.obstacles = [];
        this.cells = Array.from({length: sizeX}, (_, x) =>
            Array.from({length: sizeY}, (_, y) =>
                Array.from({length: sizeZ}, (_, z) =>
                    new Cell({x, y, z}, cellLength)
                )
            )
        );
        this.initializeConnections();
    }

    addObstacle(obstacle: Obstacle) {
        this.obstacles.push(obstacle);
        this.updateCosts();
    }

    getCells(): Cell[][][] {
        return this.cells;
    }

    getObstacles(): Obstacle[] {
        return this.obstacles;
    }

    getCellLength(): number {
        return this.cellLength
    }

    getLayerLength(): number {
        return this.layerHeight
    }

    getGridSizeX(): number {
        return this.gridSizeX;
    }

    getGridSizeY(): number {
        return this.gridSizeY;
    }

    getGridSizeZ(): number {
        return this.gridSizeZ;
    }

    findPath(start: Cell, goal: Cell): PathPoint[] {
        if (!this.isCellInGrid(start) || !this.isCellInGrid(goal)) {
            return [];
        }

        type NodeWithPriority = { cell: Cell, priority: number };
        const openSet: NodeWithPriority[] = [];
        const closedSet = new Set<Cell>();
        const cameFrom = new Map<Cell, Cell | undefined>();
        const gScore = new Map<Cell, number>();

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
                return smoothPath(path, this.cellLength);
            }

            closedSet.add(current);

            for (const neighbor of this.getValidNeighbors(current)) {
                if (closedSet.has(neighbor)) continue;
                const tentativeG = (gScore.get(current) ?? Infinity) + neighbor.getCost();
                if (!gScore.has(neighbor) || tentativeG < (gScore.get(neighbor) ?? Infinity)) {
                    cameFrom.set(neighbor, current);
                    gScore.set(neighbor, tentativeG);
                    const f = tentativeG + this.heuristic(neighbor, goal);
                    openSet.push({cell: neighbor, priority: f});
                }
            }
        }
        return [];
    }

    findRawPath(start: Cell, goal: Cell): Cell[] | null {
        if (!this.isCellInGrid(start) || !this.isCellInGrid(goal)) {
            return null;
        }
        type NodeWithPriority = { cell: Cell, priority: number };
        const openSet: NodeWithPriority[] = [];
        const closedSet = new Set<Cell>();
        const cameFrom = new Map<Cell, Cell | undefined>();
        const gScore = new Map<Cell, number>();

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
                return path;
            }

            closedSet.add(current);

            for (const neighbor of this.getValidNeighbors(current)) {
                if (closedSet.has(neighbor)) continue;
                const tentativeG = (gScore.get(current) ?? Infinity) + neighbor.getCost();
                if (!gScore.has(neighbor) || tentativeG < (gScore.get(neighbor) ?? Infinity)) {
                    cameFrom.set(neighbor, current);
                    gScore.set(neighbor, tentativeG);
                    const f = tentativeG + this.heuristic(neighbor, goal);
                    openSet.push({cell: neighbor, priority: f});
                }
            }
        }
        return null;
    }

    getCellByIndices(x: number, y: number, z: number): Cell | null {
        if (
            x >= 0 && x < this.gridSizeX &&
            y >= 0 && y < this.gridSizeY &&
            z >= 0 && z < this.gridSizeZ
        ) {
            return this.cells[x][y][z];
        }
        return null;
    }

    getCellCenter(x: number, y: number, z: number): [number, number, number] {
        const cx = this.offset[0] + (x + 0.5) * this.cellLength;
        const cy = this.offset[2] + (z + 0.5) * this.layerHeight;
        const cz = this.offset[1] + (y + 0.5) * this.cellLength;

        return [cx, cy, cz];
    }

    private isCellInGrid(cell: Cell): boolean {
        if (!cell) return false;
        const {x, y, z} = cell.getCoords();
        return (
            x >= 0 && x < this.gridSizeX &&
            y >= 0 && y < this.gridSizeY &&
            z >= 0 && z < this.gridSizeZ
        );
    }

    private getValidNeighbors(cell: Cell): Cell[] {
        const neighbors: Cell[] = [];
        const coords = cell.getCoords();

        for (let dx = -1; dx <= 1; ++dx) {
            for (let dy = -1; dy <= 1; ++dy) {
                if (dx === 0 && dy === 0) continue;
                const newX = coords.x + dx;
                const newY = coords.y + dy;
                const newZ = coords.z;
                if (
                    newX >= 0 && newX < this.gridSizeX &&
                    newY >= 0 && newY < this.gridSizeY &&
                    newZ >= 0 && newZ < this.gridSizeZ
                ) {
                    const neighbor = this.cells[newX][newY][newZ];
                    if (neighbor.getCost() !== 254.0) neighbors.push(neighbor);
                }
            }
        }

        if (isVerticalTransitionAllowed(coords)) {
            for (const dz of [-1, 1]) {
                const nz = coords.z + dz;
                if (nz >= 0 && nz < this.gridSizeZ) {
                    const neighbor = this.cells[coords.x][coords.y][nz];
                    if (neighbor.getCost() !== 254.0) neighbors.push(neighbor);
                }
            }
        }
        return neighbors;
    }

    private initializeConnections() {
        for (let x = 0; x < this.gridSizeX; ++x) {
            for (let y = 0; y < this.gridSizeY; ++y) {
                for (let z = 0; z < this.gridSizeZ; ++z) {
                    const cell = this.cells[x][y][z];
                    if (x > 0) cell.addConnection(this.cells[x - 1][y][z]);
                    if (x < this.gridSizeX - 1) cell.addConnection(this.cells[x + 1][y][z]);
                    if (y > 0) cell.addConnection(this.cells[x][y - 1][z]);
                    if (y < this.gridSizeY - 1) cell.addConnection(this.cells[x][y + 1][z]);
                    if (z > 0) cell.addConnection(this.cells[x][y][z - 1]);
                    if (z < this.gridSizeZ - 1) cell.addConnection(this.cells[x][y][z + 1]);
                }
            }
        }
    }

    private updateCosts() {
        for (let x = 0; x < this.gridSizeX; ++x) {
            for (let y = 0; y < this.gridSizeY; ++y) {
                for (let z = 0; z < this.gridSizeZ; ++z) {
                    this.cells[x][y][z].setCost(this.calculateCellCost(x, y, z));
                }
            }
        }
    }

    private calculateCellCost(x: number, y: number, z: number): number {
        let minDistance = Number.POSITIVE_INFINITY;
        for (const obstacle of this.obstacles) {
            if (
                x >= obstacle.getXMin() && x <= obstacle.getXMax() &&
                y >= obstacle.getYMin() && y <= obstacle.getYMax() &&
                z >= obstacle.getZMin() && z <= obstacle.getZMax()
            ) {
                return 254.0;
            }
            const dx = Math.max(obstacle.getXMin() - x, 0, x - obstacle.getXMax());
            const dy = Math.max(obstacle.getYMin() - y, 0, y - obstacle.getYMax());
            const dz = Math.max(obstacle.getZMin() - z, 0, z - obstacle.getZMax());
            const distance = Math.sqrt(dx * dx + dy * dy + dz * dz);
            if (distance < minDistance) minDistance = distance;
        }
        if (minDistance === Number.POSITIVE_INFINITY) return 50.0;
        let cost = Math.exp(-this.k * (minDistance - this.droneRadius)) * 253.0;
        cost += z * this.layerHeight * this.k;
        cost = Math.max(50.0, Math.min(cost, 253.0));
        return cost;
    }

    private heuristic(a: Cell, b: Cell): number {
        const aC = a.getCoords();
        const bC = b.getCoords();
        const dx = (aC.x - bC.x) * this.cellLength;
        const dy = (aC.y - bC.y) * this.cellLength;
        const dz = (aC.z - bC.z) * this.layerHeight;
        const fv = 2.0;
        return Math.sqrt(dx * dx + dy * dy + (dz * fv) * (dz * fv));
    }

    getOffset(): [number, number, number] {
        return this.offset;
    }
}
