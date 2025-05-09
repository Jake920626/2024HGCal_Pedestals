import ROOT

# 開啟 root 檔案
file = ROOT.TFile.Open("Relay1727132796.root","read")

# 列出所有 TGraphErrors 的名字
keys = file.GetListOfKeys()

# 遍歷所有 TGraphErrors
for key in keys:
    obj = key.ReadObj()
    if isinstance(obj, ROOT.TGraphErrors):
        graph = obj
        name = graph.GetName()
        print(name)

        n = graph.GetN()
        print(n)
        for i in range(n):
            x = ROOT.Double(0)
            y = ROOT.Double(0)
            graph.GetPoint(i, x, y)
            ex = graph.GetErrorX(i)
            ey = graph.GetErrorY(i)
            print(f"  Point {i}: x = {x}, y = {y}, ey (σ) = {ey}")
