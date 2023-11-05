import speech_recognition as sr
from gtts import gTTS
import os
import openai
from dotenv import load_dotenv
import subprocess

load_dotenv()
openai.api_key = "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"

# Inicializa el reconocimiento de voz
r = sr.Recognizer()

# Función para escuchar y convertir voz en texto
def listen_and_convert():
    with sr.Microphone() as source:
        print("Escuchando...")
        audio = r.listen(source)

    try:
        print("Reconociendo...")
        text = r.recognize_google(audio, language="es-ES")
        print(text)
        return text
    except sr.UnknownValueError:
        return ""
    except sr.RequestError as e:
        print("Error en la solicitud: {0}".format(e))
        return ""

# Función para generar voz a partir de texto
def speak(text):
    tts = gTTS(text=text, lang='es')
    tts.save("output.mp3")
    #ruta = "C:\\Users\\juanm\\OneDrive\\Documentos\\GitHub\\ProyectoSmartHome\\output.mp3"
    #play(AudioSegment.from_mp3(ruta))
    #subprocess.run(['winplay', "output.mp3"], shell=True)
    os.system("mpg123 output.mp3")
    #playsound("output.mp3")
    os.remove("output.mp3")

def chat(mensaje):
    respuesta = openai.ChatCompletion.create(
        model="gpt-3.5-turbo",
        messages=[
            {"role": "system","content": "Eres un asistente de voz igual a jarvis de ironman"},
            {"role": "user","content": mensaje},
        ]
        )
    resultado = ""
    for choice in respuesta.choices:
        resultado += choice.message.content
    return resultado

#speak("Hola, soy el asistente de voz. ¿que puedo hacer por ti ?")
print(chat("hola asistente como estas?"))

while True:
    # Escucha continuamente hasta que se detecta la palabra clave
    activation_phrase = "Hola asistente"
    detected = False

    while not detected:
        voice_input = listen_and_convert()
        if activation_phrase in voice_input:
            print("Palabra clave detectada. ¡Háblame!")
            detected = True

    # Escucha y convierte el discurso en texto
    user_input = listen_and_convert()
    print("Usuario: " + user_input)

    # Puedes agregar aquí la lógica para procesar las órdenes del usuario

    # Por ejemplo, aquí simplemente repetimos lo que el usuario dijo
    speak(chat(user_input))
