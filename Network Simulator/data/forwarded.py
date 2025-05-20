import os
import json
import plotly.graph_objects as go
from plotly.subplots import make_subplots

folder_path = "./Forwarded"

forwarded_values = []
lost_values = []

for filename in os.listdir(folder_path):
    if filename.endswith(".json"):
        file_path = os.path.join(folder_path, filename)
        with open(file_path, "r") as file:
            data = json.load(file)
            forwarded_values.append(data.get("forwarded", 0))
            lost_values.append(data.get("lost", 0))

fig = make_subplots(rows=1, cols=2, subplot_titles=("Histogram of Forwarded", "Histogram of Lost"))

fig.add_trace(
    go.Histogram(x=forwarded_values, name="Forwarded", marker=dict(color='blue')),
    row=1, col=1
)

fig.add_trace(
    go.Histogram(x=lost_values, name="Lost", marker=dict(color='red')),
    row=1, col=2
)


fig.update_layout(
    title="Histograms of Forwarded and Lost",
    xaxis_title="Values",
    yaxis_title="Frequency",
    bargap=0.2,
    height=500,
    width=900
)

fig.write_html("./forwarded.html")
