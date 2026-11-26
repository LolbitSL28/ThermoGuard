import { Injectable } from '@angular/core';
import { HttpClient } from '@angular/common/http';
import { Observable, interval, switchMap } from 'rxjs';

@Injectable({
  providedIn: 'root'
})
export class Esp32Service {
  private apiUrl = 'http://10.152.178.178/data'; 

  constructor(private http: HttpClient) { }

  getDatosEnTiempoReal(): Observable<any> {
    return interval(1000).pipe(
      switchMap(() => this.http.get<any>(this.apiUrl))
    );
  }
}