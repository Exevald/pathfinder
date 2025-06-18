import React from 'react';
import ReactDOM from 'react-dom/client';
import { reatomContext } from '@reatom/npm-react';
import { createCtx } from '@reatom/core';
import './index.css';
import { Controls } from './View/Controls';
import SceneView from './View/SceneView';
import SandboxView from './View/SandboxView';
import { useAtom, useAction } from '@reatom/npm-react';
import { modeAtom, setMode } from './Model/atoms';

const ModeSelector: React.FC = () => {
    const [mode] = useAtom(modeAtom);
    const changeMode = useAction(setMode);

    return (
        <div style={{ marginBottom: 16 }}>
            <h3>Режим работы:</h3>
            <div style={{ display: 'flex', gap: 8 }}>
                <button
                    onClick={() => changeMode('obj')}
                    style={{
                        background: mode === 'obj' ? '#4CAF50' : '#f0f0f0',
                        color: mode === 'obj' ? 'white' : 'black',
                        border: 'none',
                        padding: '8px 16px',
                        borderRadius: 4,
                        cursor: 'pointer'
                    }}
                >
                    OBJ модель
                </button>
                <button
                    onClick={() => changeMode('sandbox')}
                    style={{
                        background: mode === 'sandbox' ? '#4CAF50' : '#f0f0f0',
                        color: mode === 'sandbox' ? 'white' : 'black',
                        border: 'none',
                        padding: '8px 16px',
                        borderRadius: 4,
                        cursor: 'pointer'
                    }}
                >
                    Песочница
                </button>
            </div>
        </div>
    );
};

const App: React.FC = () => {
    const [mode] = useAtom(modeAtom);

    return (
        <div style={{ display: 'flex', flexDirection: 'row', height: '100vh' }}>
            <div style={{ width: 300, padding: 16, background: '#f7f7f7', borderRight: '1px solid #eee' }}>
                <ModeSelector />
                {mode === 'obj' && <Controls />}
            </div>
            <div style={{ flex: 1, height: '100%' }}>
                {mode === 'obj' ? <SceneView /> : <SandboxView />}
            </div>
        </div>
    );
};

const root = ReactDOM.createRoot(
    document.getElementById('root') as HTMLElement
);
const ctx = createCtx();

root.render(
    <React.StrictMode>
        <reatomContext.Provider value={ctx}>
            <App />
        </reatomContext.Provider>
    </React.StrictMode>
);