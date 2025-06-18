import React, {useContext, useEffect, useState} from 'react';
import {PathfindingViewModel} from './PathfindingViewModel';

export const ViewModelContext = React.createContext<PathfindingViewModel | null>(null);

export const usePathfindingVM = (): PathfindingViewModel => {
    const viewModel = useContext(ViewModelContext);
    if (!viewModel) {
        throw new Error('usePathfindingVM must be used within a ViewModelProvider');
    }

    const [, forceUpdate] = useState(0);
    useEffect(() => {
        return viewModel.subscribe(() => forceUpdate(c => c + 1));
    }, [viewModel]);

    return viewModel;
};