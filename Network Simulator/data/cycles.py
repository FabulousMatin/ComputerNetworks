import os
import json
import plotly.graph_objects as go
from plotly.subplots import make_subplots

folder_path = "./Cycles"

waiting_values = []
total_values = []
for filename in os.listdir(folder_path):
    if filename.endswith(".json"):
        file_path = os.path.join(folder_path, filename)
        with open(file_path, "r") as file:
            data = json.load(file)
            waiting_values.append(data.get("waiting", 0))
            total_values.append(data.get("total", 0))

fig = make_subplots(rows=1, cols=2, subplot_titles=("Histogram of Forwarded", "Histogram of Lost"))

fig.add_trace(
    go.Histogram(x=waiting_values, name="Waiting", marker=dict(color='blue')),
    row=1, col=1
)

fig.add_trace(
    go.Histogram(x=total_values, name="Total", marker=dict(color='red')),
    row=1, col=2
)


fig.update_layout(
    title="Histograms of Waiting and Total Cycles",
    xaxis_title="Values",
    yaxis_title="Frequency",
    bargap=0.2,
    height=500,
    width=900
)

fig.write_html("./Cycles.html")
