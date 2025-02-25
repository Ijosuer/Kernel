import React from 'react';
import './styles/button.css';
import { useNavigate } from 'react-router-dom';


export default function Button() {
    const navigate = useNavigate();

    const IrTaskManager = () => {
        navigate('/task_manager')
    }
    return (
        <button className="btn cube cube-hover" type="button" onClick={IrTaskManager}>
            <div className="bg-top">
                <div className="bg-inner"></div>
            </div>
            <div className="bg-right">
                <div className="bg-inner"></div>
            </div>
            <div className="bg">
                <div className="bg-inner"></div>
            </div>
            <div className="text">Go Task M</div>
        </button>
    );
}
