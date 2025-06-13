import { usePathfindingVM } from '../ViewModel/PathfindingViewModel';

export const Controls = () => {
    const vm = usePathfindingVM();

    return (
        <div className="controls" style={{ zIndex: 10, position: 'relative' }}>
            <div>
                <label>
                    Загрузить OBJ:
                    <input
                        type="file"
                        accept=".obj"
                        onChange={e => {
                            const file = e.target.files?.[0];
                            if (file) vm.setObjFile(file);
                        }}
                    />
                </label>
            </div>
            <div>
                <label>
                    Загрузить MTL:
                    <input
                        type="file"
                        accept=".mtl"
                        onChange={e => {
                            const file = e.target.files?.[0];
                            if (file) vm.setMtlFile(file);
                        }}
                    />
                </label>
            </div>
            <button
                onClick={() => {
                    if (vm.objFile && vm.mtlFile) vm.loadObjMtl(vm.objFile, vm.mtlFile);
                }}
                disabled={!vm.objFile || !vm.mtlFile}
            >
                Отрисовать модель
            </button>
        </div>
    );
};