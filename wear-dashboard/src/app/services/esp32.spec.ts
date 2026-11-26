import { TestBed } from '@angular/core/testing';
import { Esp32 } from './esp32';

describe('Esp32', () => {
  let service: Esp32;

  beforeEach(() => {
    TestBed.configureTestingModule({});
    service = TestBed.inject(Esp32);
  });

  it('should be created', () => {
    expect(service).toBeTruthy();
  });
});
