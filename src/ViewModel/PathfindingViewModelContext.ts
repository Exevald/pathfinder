import React, {useContext, useEffect, useState} from 'react';
import {PathfindingViewModel} from './PathfindingViewModel';

export const PathfindingViewModelContext = React.createContext<PathfindingViewModel | null>(null);

export const usePathfindingVM = (): PathfindingViewModel => {
    const viewModel = useContext(PathfindingViewModelContext);
    if (!viewModel) {
        throw new Error('usePathfindingVM must be used within a ViewModelProvider');
    }

    const [, forceUpdate] = useState(0);
    useEffect(() => {
        return viewModel.subscribe(() => forceUpdate(c => c + 1));
    }, [viewModel]);

    return viewModel;
};