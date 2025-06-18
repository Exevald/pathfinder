import React from "react";

export type EditMode = 'move' | 'rotate' | 'scale' | null;

export const EditToolbar: React.FC<{
    editMode: EditMode;
    onModeChange: (mode: EditMode) => void;
    onDelete: () => void;
    onDuplicate: () => void;
    onReset: () => void;
}> = ({editMode, onModeChange, onDelete, onDuplicate, onReset}) => {
    return (
        <div style={{
            display: 'flex',
            flexDirection: 'column',
            gap: '8px',
            padding: '8px',
            background: '#f0f0f0',
            borderRadius: '4px',
            marginBottom: '16px'
        }}>
            <div style={{display: 'flex', gap: '8px'}}>
                <button
                    onClick={() => onModeChange('move')}
                    style={{
                        background: editMode === 'move' ? '#4CAF50' : '#fff',
                        color: editMode === 'move' ? 'white' : 'black',
                        border: '1px solid #ccc',
                        padding: '8px 16px',
                        borderRadius: '4px',
                        cursor: 'pointer'
                    }}
                >
                    Переместить
                </button>
                <button
                    onClick={() => onModeChange('scale')}
                    style={{
                        background: editMode === 'scale' ? '#4CAF50' : '#fff',
                        color: editMode === 'scale' ? 'white' : 'black',
                        border: '1px solid #ccc',
                        padding: '8px 16px',
                        borderRadius: '4px',
                        cursor: 'pointer'
                    }}
                >
                    Изменить размер
                </button>
            </div>
            <div style={{display: 'flex', gap: '8px'}}>
                <button
                    onClick={onDuplicate}
                    style={{
                        background: '#2196F3',
                        color: 'white',
                        border: 'none',
                        padding: '8px 16px',
                        borderRadius: '4px',
                        cursor: 'pointer'
                    }}
                >
                    Дублировать
                </button>
                <button
                    onClick={onReset}
                    style={{
                        background: '#FF9800',
                        color: 'white',
                        border: 'none',
                        padding: '8px 16px',
                        borderRadius: '4px',
                        cursor: 'pointer'
                    }}
                >
                    Сбросить
                </button>
                <button
                    onClick={onDelete}
                    style={{
                        background: '#ff4444',
                        color: 'white',
                        border: 'none',
                        padding: '8px 16px',
                        borderRadius: '4px',
                        cursor: 'pointer'
                    }}
                >
                    Удалить
                </button>
            </div>
        </div>
    );
};