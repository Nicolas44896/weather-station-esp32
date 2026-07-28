from fastapi import FastAPI
from fastapi.staticfiles import StaticFiles
from pydantic import BaseModel
from collections import deque
from datetime import datetime, timezone

app = FastAPI()

class Reading(BaseModel):
    temperatura: float
    presion: float

# Ring buffer: al llegar a maxlen, cada append descarta la lectura mas vieja.
historial = deque(maxlen = 500)

@app.post("/readings")
def recibir_lectura(lectura: Reading):
    registro = lectura.model_dump()
    registro["timestamp"] = datetime.now(timezone.utc).isoformat()
    historial.append(registro)

    return {"ok": True}

@app.get("/stats")
def estadisticas():
    if not historial:
        return {"muestras": 0}
    temps = [r["temperatura"] for r in historial]
    pres = [r["presion"] for r in historial]
    return {
        "muestras": len(historial),
        "temperatura": {
            "min": min(temps),
            "max": max(temps),
            "promedio": round(sum(temps) / len(temps), 2)
        },
        "presion": {
            "min": min(pres),
            "max": max(pres),
            "promedio": round(sum(pres) / len(pres), 2)
        }
    }

@app.get("/readings")
def listar_lecturas(limit: int = 100):
    # deque no soporta slicing: hay que materializarlo en lista primero.
    return list(historial)[-limit:]

app.mount("/", StaticFiles(directory="static", html=True), name="static") # Monta la carpeta static como contenido estático en la raíz del servidor.
