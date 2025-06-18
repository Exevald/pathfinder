import React, {useContext, useEffect, useState} from 'react';
import {SandboxViewModel} from './SandboxViewModel';

export const SandboxViewModelContext = React.createContext<SandboxViewModel | null>(null);

export const useSandboxVM = (): SandboxViewModel => {
    const vm = useContext(SandboxViewModelContext);
    if (!vm) {
        throw new Error('useSandboxVM must be used within a SandboxViewModelProvider');
    }

    const [, forceUpdate] = useState(0);
    useEffect(() => {
        return vm.subscribe(() => forceUpdate(c => c + 1));
    }, [vm]);

    return vm;
};