import pandas as pd
import quandl
import math
import numpy as np
from matplotlib import style
import matplotlib.pyplot as plt
import datetime

def deprecated(func):
    def wrapper(*args, **kwargs):
        print("using deprecated function {}".format(func.__name__))
        return func(*args, **kwargs)
    return wrapper

class Data:
    df = pd.DataFrame()

    def __init__(self):
        style.use("ggplot")
        with open("key.apikey", "r") as f:
            quandl.ApiConfig.api_key = f.readline().strip("\n")


    def write_parsed_forex_data(self, fx):        
        with open("{}_complete_rates.txt".format(fx), "a") as f:
            np.savetxt("{}_complete_rates.txt".format(fx), self.df["Rate"].values, fmt="%f")

        with open("{}_complete_ma12.txt".format(fx), "a") as f:
            np.savetxt("{}_complete_ma12.txt".format(fx), self.df["MA12"][11:].values, fmt="%f")

        with open("{}_complete_ma26.txt".format(fx), "a") as f:
            np.savetxt("{}_complete_ma26.txt".format(fx), self.df["MA26"][25:].values, fmt="%f")
       
        

    def stock_data(self, reload=False, name="GOOGL"):
        if reload:
            df = quandl.get("WIKI/{}".format(name), authtoken=quandl.ApiConfig.api_key)
            df.to_csv("{}.csv".format(name))
        else:
            df = pd.read_csv("{}.csv".format(name))

        print(df)
        df["HL_PCT"] = (df["Adj. High"] - df["Adj. Close"]) / df["Adj. Close"] * 100.0
        df["PCT_change"] = (df["Adj. Close"] - df["Adj. Open"]) / df["Adj. Open"] * 100.0

        df = df[["PCT_change", "Adj. Close", "Adj. High", "Adj. Low"]]

        fig, axs = plt.subplots(2)
        axs[0].plot(df[["Adj. Close", "Adj. High", "Adj. Low"]])
        axs[1].plot(df["PCT_change"])

        self.df = df

    def forex_data(self, reload, visualize, fx):
        if reload:
            df = quandl.get("CURRFX/{}".format(fx), authtoken=quandl.ApiConfig.api_key)
            df.to_csv("{}.csv".format(fx))
        else:
            df = pd.read_csv("{}.csv".format(fx))
        
        df.dropna(inplace=True)
        print(df)
        df["MA12"] = df["Rate"].rolling(window=12).mean()
        df["MA26"] = df["Rate"].rolling(window=26).mean()
        df["PCT_change"] = df["Rate"].pct_change()
        df["PCT_change"].dropna(inplace=True)
        df["MA12"].dropna(inplace=True)
        df["MA26"].dropna(inplace=True)
        
        df["Signals"] = [(1 if abs(x-y) <= 0.005 else 0) for x, y in zip(df["MA12"], df["MA26"])]

        df.to_csv("{}_complete.csv".format(fx))

        self.df = df
        
        self.write_parsed_forex_data(fx)

        if visualize: 
            fig, axs = plt.subplots(3)
            axs[0].plot(df["Rate"], label="Rate ({})".format(fx))
            axs[0].plot(df["MA12"], label="Moving Average (12)")
            axs[0].plot(df["MA26"], label="Moving Average (26)")
            axs[0].legend(loc="upper left")
            
            axs[1].plot(df["PCT_change"], label="Change")
            axs[1].legend(loc="upper left")
            
            axs[2].plot(df["Signals"])
            plt.show()

def main():
    d = Data()
    d.forex_data(reload=False, visualize=False, fx="EURHUF")
    
    
if __name__ == "__main__":
    main()


