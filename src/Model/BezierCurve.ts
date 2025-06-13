export class BezierCurve {
    private controlPoints: [number, number, number][];

    constructor(controlPoints: [number, number, number][]) {
        this.controlPoints = controlPoints;
    }

    evaluate(t: number): [number, number, number] {
        let points = this.controlPoints.map(p => [...p] as [number, number, number]);
        let n = points.length;

        while (n > 1) {
            for (let i = 0; i < n - 1; ++i) {
                points[i][0] = (1 - t) * points[i][0] + t * points[i + 1][0];
                points[i][1] = (1 - t) * points[i][1] + t * points[i + 1][1];
                points[i][2] = (1 - t) * points[i][2] + t * points[i + 1][2];
            }
            --n;
        }

        return points[0];
    }
}
