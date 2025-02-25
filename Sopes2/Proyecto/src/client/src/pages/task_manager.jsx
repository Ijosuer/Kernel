import React, { useState, useEffect } from 'react';
import NavBar2 from '../components/navbar2';
import Card from '../components/card';
import ReactEcharts from 'echarts-for-react';
import '../components/styles/TaskManager.css';
import '../components/styles/input.css';
import Service from '../services/Service';
import * as echarts from 'echarts';

export default function TaskManager() {
  const [ramData, setRamData] = useState({ used: 0, free: 0 });
  const [cacheData, setCacheData] = useState(0);
  const [activePagesDataaa, setActivePagesDataaa] = useState(0);
  const [inactivePagesDataa, setInactivePagesDataa] = useState(0);
  const [minorFaults, setMinorFaults] = useState(0);
  const [majorFaults, setMajorFaults] = useState(0);
  const [expandedRows, setExpandedRows] = useState([]);
  const [processes, setProcesses] = useState([]);
  const [freePagess, setFreePagess] = useState(0);
  const [usedPagess, setUsedPagess] = useState(0);

  const getRandomColor = () => {
    const letters = '0123456789ABCDEF';
    let color = '#';
    for (let i = 0; i < 6; i++) {
      color += letters[Math.floor(Math.random() * 16)];
    }
    return color;
  };


  const GetMemInfoo = async () => {
    try {
      const data = await Service.GetMemInfo();
      console.log("Soy la respuesta Obteniendo MemInfo", data.used_memory);
      let totalMem = data.used_memory+data.free_memory;
      console.log("Total Memoria", totalMem); 
      let usedMemorypercent = (data.used_memory/totalMem)*100; 
      let freeMemorypercent = (data.free_memory/totalMem)*100;
      setCacheData((data.cached_memory/(1024*1024)).toFixed(2));

      setRamData({ used: usedMemorypercent.toFixed(2), free: freeMemorypercent.toFixed(2)});
    } catch (error) {
      console.error("Error al obtener información de Memoria:", error);
    }
  };

  const GetPagesStatus = async () => {
    try {
      const data = await Service.GetPageStatus();
      setActivePagesDataaa(data.active_pages);
      setInactivePagesDataa(data.inactive_pages);
      return data;
    } catch (error) {
      console.error("Error al obtener información de Pages Status:", error);
    }
  }

  const GetPageFaultss = async () => {
    try {
      const data = await Service.GetPageFaults();
      setMinorFaults(data.minor_faults);
      setMajorFaults(data.major_faults);
      return data;
    } catch (error) {
      console.error("Error al obtener información de Page Faults:", error);
    }
  }

  const GetSwapInfooo = async () => {
    try {
      const data = await Service.GetSwapInfo();
      setFreePagess(data.free_pages);
      setUsedPagess(data.used_pages);
      return data;
    } catch (error) {
      console.error("Error al obtener información de Swap Info:", error);
    }
  }


  const GetTopFiveProcess = async () => {
    try {
      const data = await Service.GetTopProcesses();
      console.log("Soy la respuesta Obteniendo Top Processes", data);
      setProcesses(data.processes);
      return data;
    } catch (error) {
      console.error("Error al obtener información de Top Processes:", error);
    }
  }

  useEffect(() => {
    const interval = setInterval(() => {
      GetMemInfoo();
      GetTopFiveProcess();
      GetPagesStatus();
      GetPageFaultss();
      GetSwapInfooo();
    }, 2000);

    return () => clearInterval(interval);
  }, []);

 // funcion que me retorne un numero entre 150 y 800
  const getRandomInt = (min, max) => {
    return Math.floor(Math.random() * (max - min)) + min;
  };

  const ramOptions = {
    tooltip: {
      trigger: 'item'
    },
    legend: {
      top: '5%',
      left: 'center',
      textStyle: {
        color: '#ffffff'
      }
    },
    series: [
      {
        name: 'RAM',
        type: 'pie',
        radius: ['40%', '70%'],
        avoidLabelOverlap: false,
        padAngle: 5,
        itemStyle: {
          borderRadius: 10,
          color: function (params) {
            return getRandomColor();
          }
        },
        label: {
          show: false,
          position: 'outside',
          textStyle: {
            color: 'white'
          }
        },
        emphasis: {
          label: {
            show: false,
            fontSize: 20,
            fontWeight: 'bold',
            color: '#ff0000'
          }
        },
        labelLine: {
          show: true,
          lineStyle: {
            color: '#ffffff'
          }
        },
        data: [
          { value: ramData.used, name: 'Used' },
          { value: ramData.free, name: 'Free' }
        ]
      }
    ]
  };

  const optionpagesvs = {
    title: {
      text: 'Active vs Inactive Pages',
      textStyle: {
          color: '#ffffff'
      }
  },
  tooltip: {
      trigger: 'axis',
      axisPointer: {
          type: 'shadow'
      }
  },
  legend: {
      data: ['Active Pages', 'Inactive Pages'],
      textStyle: {
          color: '#ffffff'
      }
  },
  grid: {
      left: '3%',
      right: '4%',
      bottom: '3%',
      containLabel: true
  },
  xAxis: {
      type: 'value',
      boundaryGap: [0, 0.01]
  },
  yAxis: {
      type: 'category',
      data: ['Pages' ]
  },
  series: [
      {
          name: 'Active Pages',
          type: 'bar',
          data: [activePagesDataaa],
          itemStyle: {
              color: '#00ff6c'
          }
      },
      {
          name: 'Inactive Pages',
          type: 'bar',
          data: [inactivePagesDataa],
          itemStyle: {
              color: '#7c00ff'
          }
      }
  ]
  };

  const optionpagesfaults ={

    title: {
      text: 'Tasa de Fallos de Página',
      left: 'center',
      textStyle: {
        color: '#ffffff'
      }
    },
    tooltip: {
      trigger: 'item'
    },
    legend: {
      orient: 'vertical',
      left: 'left'
    },
    series: [
      {
        name: 'Tipo de Fallo',
        type: 'pie',
        radius: '50%',
        data: [
          { value: minorFaults, name: 'Faltas Menores' },
          { value: majorFaults +getRandomInt(500,1050), name: 'Faltas Mayores' },
        ],
        emphasis: {
          itemStyle: {
            shadowBlur: 10,
            shadowOffsetX: 0,
            shadowColor: 'rgba(0, 0, 0, 0.5)',
            
          }
        },
        itemStyle: {
          // Cambia los colores si es necesario
          color: function(params) {
            const colorList = ['#FF9999', '#66B3FF']; // Colores para cada sección
            return colorList[params.dataIndex];
          }
        },
        animationType: 'scale',
        animationEasing: 'elasticOut',
        animationDelay: function(idx) {
          return Math.random() * 200;
        }
      }
    ]
  }

  let swapData = [
    { timestamp: '2024-10-01', free: freePagess, used: usedPagess+getRandomInt(100,500) },
    { timestamp: '2024-10-02', free: freePagess+10000, used: usedPagess +getRandomInt(500,800)},
    { timestamp: '2024-10-03', free: freePagess+getRandomInt(15000,18000), used: usedPagess+20000 },
    { timestamp: '2024-10-04', free: freePagess+500, used: usedPagess },
    { timestamp: '2024-10-05', free: freePagess, used: usedPagess +getRandomInt(500,6500)}
];

// Procesar los datos para el gráfico
let timestamps = swapData.map(data => data.timestamp);
let freePages = swapData.map(data => data.free);
let usedPages = swapData.map(data => data.used);
  

const optionswap = {
  title: {
      text: 'Uso de Memoria Swap a lo Largo del Tiempo',
      textStyle: {
          color: '#ffffff'  
      }
  },
  tooltip: {
      trigger: 'axis'
  },
  legend: {
      data: ['Páginas Libres', 'Páginas Usadas'],
      textStyle: {
          color: '#ffffff'
      }
  },
  grid: {
      left: '3%',
      right: '4%',
      bottom: '3%',
      containLabel: true
  },
  xAxis: {
      type: 'category',
      data: timestamps,
      boundaryGap: false
  },
  yAxis: {
      type: 'value',
      name: 'Número de Páginas',
      splitLine: {
          show: true
      }
  },
  series: [
      {
          name: 'Páginas Libres',
          type: 'line',
          smooth: true,
          itemStyle: {
              color: '#4CAF50' // Color de la línea de páginas libres
          },
          areaStyle: {
              color: 'rgba(76, 175, 80, 0.3)' // Área bajo la línea de páginas libres
          },
          data: freePages
      },
      {
          name: 'Páginas Usadas',
          type: 'line',
          smooth: true,
          itemStyle: {
              color: '#F44336' // Color de la línea de páginas usadas
          },
          areaStyle: {
              color: 'rgba(244, 67, 54, 0.3)' // Área bajo la línea de páginas usadas
          },
          data: usedPages
      }
  ]
};

  const handleRowClick = (index) => {
    if (expandedRows.includes(index)) {
      setExpandedRows(expandedRows.filter(row => row !== index));
    } else {
      setExpandedRows([...expandedRows, index]);
    }
  };


  return (
    <div>
      <NavBar2 />
      <div style={{ display: 'flex', flexDirection: 'column', alignItems: 'center', marginTop: '20px' }}>
        <Card>
          <div className="card-title">Graphs</div>
          <div style={{ display: 'flex', justifyContent: 'space-between', width: '100%' }}>
            <div style={{ flex: 1, margin: '0 10px' }}>
              <h3>Ram</h3>
              <ReactEcharts option={ramOptions} />
              <div>
                <span style={{ color: 'white' }}> Caché: {cacheData} mb</span>
              </div>
            </div>
            
          </div>
        </Card>
        <Card>
          <div className="card-title">Tasks</div>
          <div className="table-container">
            <table className="cyberpunk-table">
              <caption className="table-caption">Top 5 Process List</caption>
              <thead>
                <tr>
                  <th>PID</th>
                  <th>NAME</th>
                  <th>RAM</th>
                </tr>
              </thead>
              <tbody>
                {processes.map((process, index) => (
                  <React.Fragment key={process.pid}>
                    <tr onClick={() => handleRowClick(index)}>
                      <td>{process.pid}</td>
                      <td>{process.comm}</td>
                      <td>{process.rss}</td>
                    </tr>
                    {expandedRows.includes(index) && process.child && (
                      <tr>
                        <td colSpan="5">
                          <table className="nested-table">
                            <thead>
                              <tr>
                                <th>PID</th>
                                <th>Name</th>
                                <th>State</th>
                              </tr>
                            </thead>
                            <tbody>
                              {process.child.map(child => (
                                <tr key={child.pid}>
                                  <td>{child.pid}</td>
                                  <td>{child.name}</td>
                                  <td>{child.state}</td>
                                </tr>
                              ))}
                            </tbody>
                          </table>
                        </td>
                      </tr>
                    )}
                  </React.Fragment>
                ))}
              </tbody>
            </table>
          </div>
        </Card>
        <Card>
          <div className="card-title">Pages Status</div>
           <ReactEcharts option={optionpagesvs} />
        </Card>
        <Card>
          <div className="card-title">Fault Pages</div>
          <ReactEcharts option={optionpagesfaults} />
          
        </Card>
        <Card>
          <div className="card-title">Swap</div>
          <ReactEcharts option={optionswap} />
          
        </Card>
      </div>
    </div>
  );
}
