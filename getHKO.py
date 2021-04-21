import pandas as pd
import numpy as np

temp = pd.read_csv('https://data.weather.gov.hk/weatherAPI/hko_data/regional-weather/latest_1min_temperature.csv')
hum = pd.read_csv('https://data.weather.gov.hk/weatherAPI/hko_data/regional-weather/latest_1min_humidity.csv')

tempAverage = np.mean(temp['Air Temperature(degree Celsius)'])
humAverage = np.mean(hum['Relative Humidity(percent)'])
print("Average temp: ", tempAverage)
print("Average humidity: ", humAverage)