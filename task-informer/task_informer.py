import psutil, mmap, time
from tkinter import *
import tkinter.ttk as ttk

#import psutil, mmap, time
#from Tkinter import *
#import ttk

class Task(Frame):
    def __init__(self, parent):
        Frame.__init__(self, parent, background="white")
        self.parent = parent
        self.initUI()
        j=0
        self.nouids=False
        self.getInfo()
        self._setup_widgets()
        while(True):
            self.getInfo()
            j=j+1
            self._build_tree()
            self.container.update()
            time.sleep(10)
            del(self.processes[:])
            self.tree.delete(*self.tree.get_children())


    def initUI(self):
        self.parent.title("Process info")
        self.container = ttk.Frame()
        self.text = Text()
        self.text.insert(INSERT,"VM: " + str(psutil.virtual_memory())+'\n')
        self.text.insert(END,"Swap: " + str(psutil.swap_memory())+'\n')
        self.text.insert(END,"Page size: " + str(mmap.PAGESIZE))
        self.processes = []
        self.container.pack(fill=BOTH,expand=1)
        self.text.pack(fill=BOTH)

    def getInfo(self):
        for proc in psutil.process_iter():
            try:
                pinfo = proc.as_dict(attrs=['pid', 'name','uids','status','memory_percent'])
                try:
                    pinfo['cpu_percent']=proc.cpu_percent(interval=None)
                except psutil.AccessDenied:
                    pinfo['cpu_percent']="---"
            except psutil.NoSuchProcess:
                pass
            except AttributeError:
                pinfo = proc.as_dict(attrs=['pid', 'name', 'status','memory_percent'])
                self.nouids = True
                try:
                    pinfo['cpu_percent']=proc.cpu_percent(interval=None)
                except psutil.AccessDenied:
                    pinfo['cpu_percent']="---"
                if(pinfo["memory_percent"]==None):
                    pinfo["memory_percent"]="---"
                p_tuple=(pinfo["pid"],pinfo["name"],pinfo['cpu_percent'],pinfo['status'],pinfo['memory_percent'])
                self.processes.append(p_tuple)
            else:
                if(pinfo["memory_percent"]==None):
                    pinfo["memory_percent"]="---"
                p_tuple=(pinfo["pid"],pinfo["name"],pinfo['cpu_percent'],pinfo['status'],pinfo['memory_percent'],pinfo['uids'])
                self.processes.append(p_tuple)
        if(self.nouids):
            self.col_header=["Pid",'Name','CPU%','Status','MEM%']
        else:
            self.col_header=["Pid",'Name','CPU%','Status','MEM%','Uid']

    def _setup_widgets(self):
        self.tree = ttk.Treeview(columns=self.col_header, show="headings")
        vsb = ttk.Scrollbar(orient="vertical",command=self.tree.yview)
        hsb = ttk.Scrollbar(orient="horizontal",command=self.tree.xview)
        self.tree.configure(yscrollcommand=vsb.set,xscrollcommand=hsb.set)
        self.tree.grid(column=0, row=0, sticky='nsew', in_=self.container)
        vsb.grid(column=1, row=0, sticky='ns', in_=self.container)
        hsb.grid(column=0, row=1, sticky='ew', in_=self.container)
        self.container.grid_columnconfigure(0, weight=1)
        self.container.grid_rowconfigure(0, weight=1)

    def _build_tree(self):
        for col in self.col_header:
            self.tree.heading(col, text=col.title(), command=lambda c=col: self.sortby(self.tree, c, 1))
            self.tree.column(col, width=int(800/5))

        for item in self.processes:
            self.tree.insert('', 'end', values=item)

    def sortby(self,tree, col, descending):
        data = [(float(tree.set(child, col)), child) for child in tree.get_children('')]
        data.sort(reverse=descending)
        for ix, item in enumerate(data):
            tree.move(item[1], '', ix)
        tree.heading(col, command=lambda col=col: self.sortby(tree, col, int(not descending)))


root = Tk()
root.geometry("800x800")
app = Task(root)
root.mainloop()
