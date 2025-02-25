import { createBrowserRouter } from "react-router-dom";

import HomePage from '../pages/home';
import TaskManager from "../pages/task_manager";


export const router = createBrowserRouter([
    { 
        path: '/', 
        element: <HomePage/> 
    },
    {   path: '/task_manager', 
        element: <TaskManager/> 
    },
]);