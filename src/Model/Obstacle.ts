export class Obstacle {
    private readonly xMin: number;
    private readonly xMax: number;
    private readonly yMin: number;
    private readonly yMax: number;
    private readonly zMin: number;
    private readonly zMax: number;

    constructor(xMin: number, xMax: number, yMin: number, yMax: number, zMin: number, zMax: number) {
        this.xMin = xMin;
        this.xMax = xMax;
        this.yMin = yMin;
        this.yMax = yMax;
        this.zMin = zMin;
        this.zMax = zMax;
    }

    getXMin(): number {
        return this.xMin;
    }

    getXMax(): number {
        return this.xMax;
    }

    getYMin(): number {
        return this.yMin;
    }

    getYMax(): number {
        return this.yMax;
    }

    getZMin(): number {
        return this.zMin;
    }

    getZMax(): number {
        return this.zMax;
    }
}
