from fastapi import FastAPI
from pydantic import BaseModel

app = FastAPI()

class readings(BaseModel):
    temperatura: float
    humedad: float
    presion: float

@app.post("/readings")
def recibir_lectura(lectura: readings):
    print(lectura)
    return {"ok": True}