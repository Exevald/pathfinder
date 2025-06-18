import {usePathfindingVM} from "../ViewModel/PathfindingViewModelContext";

export const Controls = () => {
    const viewModel = usePathfindingVM();

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
                            if (file) viewModel.setObjFile(file);
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
                            if (file) viewModel.setMtlFile(file);
                        }}
                    />
                </label>
            </div>
            <button
                onClick={() => {
                    if (viewModel.getObjFile && viewModel.getMtlFile) viewModel.loadObjMtl(viewModel.getObjFile, viewModel.getMtlFile);
                }}
                disabled={!viewModel.getObjFile || !viewModel.getMtlFile}
            >
                Отрисовать модель
            </button>
        </div>
    );
};