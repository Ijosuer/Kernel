// navbar.jsx
import Imagex from '../assets/hacker.png';
import './styles/navvv.css';
import { useNavigate } from 'react-router-dom';

export default function Navbarr() {
  const navigate = useNavigate();

  const IrTaskManager = () => {
    navigate('/task_manager');
  };

  const IrHome = () => {
    navigate('/');
  };

  return (
    <nav className="navbarr">
      <div className="navbarr-brand-container">
        <img src={Imagex} alt="image" />
        <div className="navbarr-brand">Task Manager</div>
      </div>
      <ul className="navbarr-links">
        <li className="navbarr-item"><a href="#home" onClick={IrHome} className="navbarr-link">Home</a></li>
        <li className="navbarr-item"><a href="#about" onClick={IrTaskManager} className="navbarr-link">Task</a></li>
      </ul>
    </nav>
  );
}
