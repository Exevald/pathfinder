import React, { useRef, useEffect } from 'react';
import { useFrame, useThree } from '@react-three/fiber';
import * as THREE from 'three';

export const FPSMovement: React.FC<{ enabled: boolean }> = ({ enabled }) => {
    const { camera } = useThree();
    const move = useRef({ forward: false, backward: false, left: false, right: false, up: false });
    const speed = 0.1;

    useEffect(() => {
        if (!enabled) return;

        const handleKeyDown = (e: KeyboardEvent) => {
            switch (e.code) {
                case 'KeyW': move.current.forward = true; break;
                case 'KeyS': move.current.backward = true; break;
                case 'KeyA': move.current.left = true; break;
                case 'KeyD': move.current.right = true; break;
                case 'Space': move.current.up = true; break;
            }
        };
        const handleKeyUp = (e: KeyboardEvent) => {
            switch (e.code) {
                case 'KeyW': move.current.forward = false; break;
                case 'KeyS': move.current.backward = false; break;
                case 'KeyA': move.current.left = false; break;
                case 'KeyD': move.current.right = false; break;
                case 'Space': move.current.up = false; break;
            }
        };

        window.addEventListener('keydown', handleKeyDown);
        window.addEventListener('keyup', handleKeyUp);
        return () => {
            window.removeEventListener('keydown', handleKeyDown);
            window.removeEventListener('keyup', handleKeyUp);
        };
    }, [enabled]);

    useFrame(() => {
        if (!enabled) return;
        const direction = new THREE.Vector3();
        camera.getWorldDirection(direction);

        const moveVector = new THREE.Vector3();
        if (move.current.forward) moveVector.add(direction);
        if (move.current.backward) moveVector.sub(direction);

        const right = new THREE.Vector3();
        right.crossVectors(camera.up, direction).normalize();
        if (move.current.right) moveVector.sub(right);
        if (move.current.left) moveVector.add(right);

        if (move.current.up) moveVector.y += 1;

        if (moveVector.lengthSq() > 0) {
            moveVector.normalize().multiplyScalar(speed);
            camera.position.add(moveVector);
        }
    });

    return null;
};