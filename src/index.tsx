import React from 'react';
import ReactDOM from 'react-dom/client';
import { reatomContext } from '@reatom/npm-react';
import { createCtx } from '@reatom/core';
import './index.css';
import { Controls } from './View/Controls';
import SceneView from './View/SceneView';

const root = ReactDOM.createRoot(
    document.getElementById('root') as HTMLElement
);
const ctx = createCtx();

root.render(
    <React.StrictMode>
        <reatomContext.Provider value={ctx}>
            <div style={{ display: 'flex', flexDirection: 'row', height: '100vh' }}>
                <div style={{ width: 300, padding: 16, background: '#f7f7f7', borderRight: '1px solid #eee' }}>
                    <Controls />
                </div>
                <div style={{ flex: 1, height: '100%' }}>
                    <SceneView />
                </div>
            </div>
        </reatomContext.Provider>
    </React.StrictMode>
);