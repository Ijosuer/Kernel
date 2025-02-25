import Container from 'react-bootstrap/Container';
import Nav from 'react-bootstrap/Nav';
import Navbar from 'react-bootstrap/Navbar';
import NavDropdown from 'react-bootstrap/NavDropdown';
import Imagex from '../assets/hacker.png';
import './styles/navbar2.css';
import { useNavigate } from 'react-router-dom';

export default function NavBar2() {
  const navigate = useNavigate();

  const IrTaskManager = () => {
    navigate('/task_manager')
  }

  const IrHome = () => {
    navigate('/')
  }

  return (
    <nav className="navbar">
      <div className="navbar-brand-container">
        <img src={Imagex} alt="image" />
        <div className="navbar-brand">Task Manager</div>
      </div>
      <ul className="navbar-links">
        <li className="navbar-item"><a href="#home" onClick={IrHome} className="navbar-link">Home</a></li>
        <li className="navbar-item"><a href="#about" onClick={IrTaskManager}className="navbar-link">Task</a></li>
      </ul>
    </nav>
  );
}
