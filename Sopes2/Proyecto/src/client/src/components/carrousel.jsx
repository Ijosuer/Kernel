// carrousel.jsx
import React, { useState, useEffect } from 'react';
import Image1 from '../images/1.png';
import Image2 from '../images/2.jpg';
import Image3 from '../images/3.png';
import Image4 from '../images/4.jpeg';
import Image5 from '../images/5.png';
import Image6 from '../images/6.png';
import Image7 from '../images/7.png';
import './styles/carrousel.css';

const images = [Image1, Image2, Image3, Image4, Image5,Image6,Image7];

export default function PaginaInicio() {
    const [currentImage, setCurrentImage] = useState(0);

    const prevImage = () => {
        setCurrentImage(prev => (prev === 0 ? images.length - 1 : prev - 1));
    };

    const nextImage = () => {
        setCurrentImage(prev => (prev === images.length - 1 ? 0 : prev + 1));
    };

    useEffect(() => {
        const interval = setInterval(nextImage, 2500); // Cambia la imagen cada 2.5 segundos (2500 ms)

        return () => clearInterval(interval); // Limpia el intervalo cuando el componente se desmonta
    }, []);

    return (
        <div className="carousel-container">
            {images.map((image, index) => (
                <img
                    key={index}
                    src={image}
                    alt={`Image ${index + 1}`}
                    className={`carousel-image ${index === currentImage ? 'active' : ''}`}
                />
            ))}
            <button
                className="carousel-button prev"
                onClick={prevImage}>
                <svg xmlns="http://www.w3.org/2000/svg" className="h-6 w-6" fill="none" viewBox="0 0 24 24" stroke="currentColor">
                    <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M15 19l-7-7 7-7" />
                </svg>
            </button>
            <button
                className="carousel-button next"
                onClick={nextImage}>
                <svg xmlns="http://www.w3.org/2000/svg" className="h-6 w-6" fill="none" viewBox="0 0 24 24" stroke="currentColor">
                    <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M9 5l7 7-7 7" />
                </svg>
            </button>
        </div>
    );
}
