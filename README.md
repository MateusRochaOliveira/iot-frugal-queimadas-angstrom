# iot-frugal-queimadas-angstrom
Sistema IoT frugal e embarcado com ESP32 e sensor DHT11 para predição local do risco de queimadas via Índice de Ångström. Projeto apresentado no I CCIT UNIFEI Itabira.
# Sistema IoT Frugal e Embarcado para Predição de Risco de Queimadas via Índice de Ångström

**Aplicação em Santa Bárbara - MG (Região do Médio Piracicaba)**  
*Trabalho apresentado no I Congresso de Ciência, Inovação e Tecnologia — UNIFEI Campus Itabira (2026)*

* **Autor:** Mateus Rocha Oliveira (Graduando em Engenharia Elétrica - UNIFEI)
* **Orientador:** Prof. Bruno Renó Gama

---

## 🔗 Acesso Rápido
* **Aplicação Web (PWA Interativo):** [https://mateusrochaoliveira.github.io/iot-frugal-queimadas-angstrom/](https://mateusrochaoliveira.github.io/iot-frugal-queimadas-angstrom/)

## 📐 Metodologia e Índice de Ångström
O nó sensor utiliza um microcontrolador **ESP32** acoplado ao sensor **DHT11 (GPIO 23)** para aquisição microclimática e processamento local (*Edge Computing*), calculando de forma autónoma o Índice de Ångström ($I$):

$$I = \frac{R}{20} + \frac{27 - T}{10}$$

| Valor do Índice ($I$) | Classificação do Risco |
| :--- | :--- |
| $I > 4,0$ | Improvável / Baixo |
| $2,5 < I \le 4,0$ | Moderado |
| $2,0 < I \le 2,5$ | Alto |
| $I \le 2,0$ | Muito Alto / Extremo |

## 📂 Estrutura do Repositório
* `index.html`, `manifest.json`, `sw.js`: Interface PWA com suporte a dados históricos (CSV), Web Serial API (USB) e Wi-Fi.
* `firmware/esp32_angstrom_iot.ino`: Código embarcado em C++ para ESP32.
* `dados/ensaio_santa_barbara_31_10.csv`: Série experimental real coletada em Santa Bárbara - MG (1.806 amostras contínuas / 61 minutos).
