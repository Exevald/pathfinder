import {Coords} from './Coords';

export class Cell {
    private readonly coords: Coords;
    private readonly cellLength: number;
    private cost: number;
    private readonly connections: Cell[];

    constructor(coords: Coords, cellLength: number) {
        this.coords = coords;
        this.cellLength = cellLength;
        this.cost = 1.0;
        this.connections = [];
    }

    addConnection(neighbor: Cell) {
        this.connections.push(neighbor);
    }

    setCost(cost: number) {
        this.cost = cost;
    }

    getCoords(): Coords {
        return this.coords;
    }

    getConnections(): Cell[] {
        return this.connections;
    }

    getCost(): number {
        return this.cost;
    }

    getCellLength(): number {
        return this.cellLength;
    }
}
