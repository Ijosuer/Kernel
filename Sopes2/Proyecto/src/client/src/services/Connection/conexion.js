import axios from "axios";

const instance = axios.create({
    baseURL: "http://192.168.71.129:5000",
    headers: {
        "Content-Type": "application/json",
    }
});



export const GetMemInfo = async () => {
    try {
        const response = await instance.get("/memory_info");
        console.log("Soy la respuesta Obteniendo MemInfo", response.data);
        return response.data;
    } catch (error) {
        console.error("Error al obtener información de Memoria:", error);
        throw error;
    }
}



export const GetSwapInfo = async () => {
    try {
        const response = await instance.get("/swap_info");
        console.log("Soy la respuesta Obteniendo Swap Info", response.data);
        return response.data;
    } catch (error) {
        console.error("Error al obtener información de Swap Info:", error);
        throw error;
    }
}

export const GetPageFaults = async () => {
    try {
        const response = await instance.get("/page_faults");
        console.log("Soy la respuesta Obteniendo Page Faults", response.data);
        return response.data;
    } catch (error) {
        console.error("Error al obtener información de Page Faults:", error);
        throw error;
    }
}


export const GetPageStatus = async () => {
    try {
        const response = await instance.get("/page_status");
        console.log("Soy la respuesta Obteniendo Page Status", response.data);
        return response.data;
    } catch (error) {
        console.error("Error al obtener información de Page Status:", error);
        throw error;
    }
}



export const GetTopProcesses = async () => {
    try {
        const response = await instance.get("/top_processes");
        console.log("Soy la respuesta Obteniendo Top Processes", response.data);
        return response.data;
    } catch (error) {
        console.error("Error al obtener información de Top Processes:", error);
        throw error;
    }
}
