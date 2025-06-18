import React from 'react';

interface CameraControlsProps {
    controlsEnabled: boolean;
    onEnter3D: () => void;
}

export const CameraControls: React.FC<CameraControlsProps> = ({controlsEnabled, onEnter3D}) => {
    return (
        <>
            {!controlsEnabled && (
                <button
                    style={{
                        position: 'absolute', left: '50%', top: '50%',
                        transform: 'translate(-50%, -50%)', zIndex: 2,
                        padding: '1em 2em', fontSize: '1.2em', cursor: 'pointer'
                    }}
                    onClick={onEnter3D}
                >
                    Войти в режим управления камерой
                </button>
            )}
            {controlsEnabled && (
                <div style={{
                    position: 'absolute', left: '50%', top: '50%',
                    transform: 'translate(-50%, -50%)', zIndex: 10,
                    pointerEvents: 'none', width: 12, height: 12,
                    borderRadius: '50%', background: 'rgba(0,0,0,0.7)',
                    border: '2px solid white', boxSizing: 'border-box'
                }}/>
            )}
        </>
    );
};