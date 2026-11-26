import { Component, OnInit, OnDestroy, ChangeDetectorRef } from '@angular/core';
import { CommonModule } from '@angular/common';
import { Esp32Service } from '../services/esp32';
import { Chart, registerables } from 'chart.js';
import { Subscription } from 'rxjs';

Chart.register(...registerables);

@Component({
  selector: 'app-dashboard',
  standalone: true,
  imports: [CommonModule],
  templateUrl: './dashboard.html',
  styleUrls: ['./dashboard.css']
})
export class DashboardComponent implements OnInit, OnDestroy {
  datos: any = { temp: 0, bpm: 0, ir: 0, alerta: false, umbral_bpm: 120, umbral_temp: 38.5 };
  private subscripcion!: Subscription;
  today: Date = new Date();
  public chart: any;
  private apiUrl = 'http://10.152.178.178';

  constructor(private esp32Service: Esp32Service, private cdr: ChangeDetectorRef) {}

  ngOnInit(): void {
    this.crearGrafico();

    this.subscripcion = this.esp32Service.getDatosEnTiempoReal().subscribe({
      next: (res) => {
        this.datos = { ...res }; 
        this.actualizarGrafico(res.temp);
        
        this.cdr.detectChanges(); 
      },
      error: (err) => console.error('Error conectando al ESP32:', err)
    });
  }

  crearGrafico() {
    const ctx = document.getElementById('miGrafico') as HTMLCanvasElement;
    this.chart = new Chart(ctx, {
      type: 'line',
      data: {
        labels: [],
        datasets: [{
          label: 'Temperatura (°C)',
          data: [],
          borderColor: '#a855f7', // Línea morada neón
          backgroundColor: 'rgba(168, 85, 247, 0.2)',
          tension: 0.4,
          fill: true
        }]
      },
      options: {
        responsive: true,
        maintainAspectRatio: false,
        plugins: {
          legend: {
            onClick: () => {},
            labels: { color: '#ffffff' } // Texto de leyenda blanco
          }
        },
        scales: {
          x: { 
            ticks: { color: '#a1a1aa' }, // Texto eje X gris
            grid: { display: false }
          },
          y: { 
            min: 20, max: 42,
            ticks: { color: '#a1a1aa' }, // Texto eje Y gris
            grid: { color: '#2a2a35' }   // Líneas de guía sutiles
          }
        }
      }
    });
  }

  silenciarAlarma() {
    fetch(`${this.apiUrl}/silenciar`).catch(err => console.error(err));
  }

    actualizarUmbral(event: any) {
    const nuevoUmbral = event.target.value;
    this.datos.umbral_temp = parseFloat(nuevoUmbral); 
    fetch(`${this.apiUrl}/config?temp=${nuevoUmbral}`).catch(err => console.error(err));
  }

  actualizarBpm(event: any) {
  const nuevoBpm = event.target.value;
  this.datos.umbral_bpm = parseInt(nuevoBpm,10);
  fetch(`${this.apiUrl}/config?bpm=${nuevoBpm}`).catch(err => console.error(err));
}

  actualizarGrafico(temp: number) {
    if (this.chart.data.labels!.length > 20) {
      this.chart.data.labels!.shift();
      this.chart.data.datasets[0].data.shift();
    }
    this.chart.data.labels!.push(new Date().toLocaleTimeString());
    this.chart.data.datasets[0].data.push(temp);
    this.chart.update();
  }

  ngOnDestroy(): void {
    if (this.subscripcion) this.subscripcion.unsubscribe();
    if (this.chart) this.chart.destroy();
  }
}