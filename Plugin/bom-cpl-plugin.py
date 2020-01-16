 #!
# -*- coding: utf-8 -*-


__author__ = "prrvchr@gmail.com"
__copyright__ = "Copyright 2020, prrvchr"
__license__ = "Mozilla Public License v2 or GNU Lesser General Public License v3"
__version__ = "0.0.3"


"""
@package
Generates a BOM or CPL csv file compatible with:
    * JLCPcb SMT Assembly service (BOM and CPL file)
    * LCSC BOM Service (BOM file)
    * Otherwise a csv file corresponding to the following characteristics

    Functionality:
        * Generate a comma separated value BOM list (csv file type).
        * Components are sorted by Reference
        * Components are grouped by same 'PartNumber', 'SupplierRef'
        * One value per line
        * Eeschema required customs fields are:
            'Manufacturer', 'PartNumber', 'Supplier', 'SupplierRef'
        * Require custom field 'Supplier' must be set (case insensitive)
        * Default csv fields are:
            'Quantity', 'Manufacture Part Number', 'Manufacturer', 'Description', 'Supplier Part Number', 'Package'

    Usage:
        python "full_path/jlcpcb-bom-plugin.py" "%I" "%O" Quantity=1
"""


import os
import sys
import csv

from xml.etree import ElementTree
from collections import OrderedDict


g_bomext = 'BOM'
g_cplext = 'CPL'
g_posfiles = ('all-pos', 'top-pos', 'bottom-pos')

g_suppliers = {}
g_suppliers['Default'] = {}
g_suppliers['Default']['fields'] = OrderedDict()
g_suppliers['Default']['fields']['Quantity'] = 'Quantity'
g_suppliers['Default']['fields']['Manufacture Part Number'] = 'PartNumber'
g_suppliers['Default']['fields']['Manufacturer'] = 'Manufacturer'
g_suppliers['Default']['fields']['Description'] = 'ref'
g_suppliers['Default']['fields']['Supplier Part Number'] = 'SupplierRef'
g_suppliers['Default']['fields']['Package'] = 'footprint'
g_suppliers['Default']['delimiter'] = ','
g_suppliers['Default']['quotechar'] = '"'
g_suppliers['Default']['quoting'] = csv.QUOTE_MINIMAL
g_suppliers['Default']['equal'] = ('PartNumber', 'SupplierRef')
g_suppliers['Default']['sorted'] = ('Manufacturer', 'PartNumber')
g_suppliers['Default']['grouped'] = True
g_suppliers['Default']['generatecpl'] = False

g_suppliers['LCSC'] = {}
g_suppliers['LCSC']['fields'] = OrderedDict()
g_suppliers['LCSC']['fields']['Quantity'] = 'Quantity'
g_suppliers['LCSC']['fields']['Manufacture Part Number'] = 'PartNumber'
g_suppliers['LCSC']['fields']['Manufacturer'] = 'Manufacturer'
g_suppliers['LCSC']['fields']['Description'] = 'ref'
g_suppliers['LCSC']['fields']['LCSC Part Number'] = 'SupplierRef'
g_suppliers['LCSC']['fields']['Package'] = 'footprint'
g_suppliers['LCSC']['delimiter'] = ','
g_suppliers['LCSC']['quotechar'] = '"'
g_suppliers['LCSC']['quoting'] = csv.QUOTE_MINIMAL
g_suppliers['LCSC']['equal'] = ('PartNumber', 'SupplierRef')
g_suppliers['LCSC']['sorted'] = ('Manufacturer', 'PartNumber')
g_suppliers['LCSC']['grouped'] = True
g_suppliers['LCSC']['generatecpl'] = False

g_suppliers['JLCPCB'] = {}
g_suppliers['JLCPCB']['fields'] = OrderedDict()
g_suppliers['JLCPCB']['fields']['Comment'] = 'value'
g_suppliers['JLCPCB']['fields']['Designator'] = 'ref'
g_suppliers['JLCPCB']['fields']['Footprint'] = 'footprint'
g_suppliers['JLCPCB']['fields']['LCSC Part #'] = 'SupplierRef'
g_suppliers['JLCPCB']['delimiter'] = ','
g_suppliers['JLCPCB']['quotechar'] = '"'
g_suppliers['JLCPCB']['quoting'] = csv.QUOTE_MINIMAL
g_suppliers['JLCPCB']['equal'] = ('ref', )
g_suppliers['JLCPCB']['sorted'] = ('SupplierRef', )
g_suppliers['JLCPCB']['grouped'] = False
g_suppliers['JLCPCB']['generatecpl'] = True
g_suppliers['JLCPCB']['cplheaders'] = {0: 'Designator',
                                       3: 'Mid X',
                                       4: 'Mid Y',
                                       5: 'Rotation',
                                       6: 'Layer'}


def getEqual(supplier):
    if supplier in g_suppliers:
        return g_suppliers[supplier]['equal']
    return g_suppliers['Default']['equal']

def getSorted(supplier):
    if supplier in g_suppliers:
        return g_suppliers[supplier]['sorted']
    return g_suppliers['Default']['sorted']

def getValid(supplier):
    return getEqual(supplier) + getSorted(supplier)

def needGrouping(supplier):
    if supplier in g_suppliers:
        return g_suppliers[supplier]['grouped']
    return g_suppliers['Default']['grouped']

def getFields(supplier):
    if supplier in g_suppliers:
        return g_suppliers[supplier]['fields']
    return g_suppliers['Default']['fields']

def getDelimiter(supplier):
    if supplier in g_suppliers:
        return g_suppliers[supplier]['delimiter']
    return g_suppliers['Default']['delimiter']

def getQuotechar(supplier):
    if supplier in g_suppliers:
        return g_suppliers[supplier]['quotechar']
    return g_suppliers['Default']['quotechar']

def getQuoting(supplier):
    if supplier in g_suppliers:
        return g_suppliers[supplier]['quoting']
    return g_suppliers['Default']['quoting']

def needCpl(supplier):
    if supplier in g_suppliers:
        return g_suppliers[supplier]['generatecpl']
    return g_suppliers['Default']['generatecpl']

def getHeaders(supplier):
    if supplier in g_suppliers:
        return g_suppliers[supplier]['cplheaders']
    return ()

def getInputs(path):
    inputs = []
    for post in g_posfiles:
        inputs.append(getInput(path, post))
    return inputs

def getInput(path, post, ext='csv'):
    return "%s-%s.%s" % (path, post, ext)

def getOutput(path, supplier, post, ext='csv'):
    return "%s_%s_%s.%s" % (path, supplier.title(), post, ext)

def getQuantity(quantity, minimum=0):
    try:
        i = int(quantity)
    except ValueError:
        i = minimum
    return max(i, minimum)


class Component(object):
    def __init__(self, component):
        self.ref = component.attrib['ref']
        value = component.find('value')
        if value is not None:
            self.value = value.text
        else:
            self.value = ''
        footprint = component.find('footprint')
        if footprint is not None:
            self.footprint = footprint.text.split(':').pop()
        else:
            self.footprint = ''
        supplier = component.find('./fields/field[@name="Supplier"]')
        if supplier is not None:
            self._supplier = supplier.text
        else:
            self._supplier = ''
        self.Manufacturer = ''
        self.PartNumber = ''
        self.SupplierRef = ''
        self.Quantity = 1

    @property
    def isvalid(self):
        if self._supplier != '':
            self.Supplier = self._supplier.upper()
            self._sorted = getSorted(self.Supplier)
            self._equal = getEqual(self.Supplier)
            return True
        return False

    def __eq__(self, other):
        if self.Supplier != other.Supplier:
            return False
        for a in self._equal:
            if getattr(self, a) != getattr(other, a):
                return False
        return True

    def __lt__(self, other):
        if self.Supplier != other.Supplier:
            return self.Supplier < other.Supplier
        for a in self._sorted:
            if getattr(self, a) != getattr(other, a):
                return getattr(self, a) < getattr(other, a)
        return False

    def setCustomFields(self, fields, suppliers):
        manufacturer = fields.find('./field[@name="Manufacturer"]')
        if manufacturer is not None:
            self.Manufacturer = manufacturer.text
        partnumber = fields.find('./field[@name="PartNumber"]')
        if partnumber is not None:
            self.PartNumber = partnumber.text
        reference = fields.find('./field[@name="%sRef"]' % self._supplier)
        if reference is not None:
            self.SupplierRef = reference.text
        else:
            reference = fields.find('./field[@name="SupplierRef"]')
            if reference is not None:
                self.SupplierRef = reference.text
        quantity = fields.find('./field[@name="Quantity"]')
        if quantity is not None:
            self.Quantity = getQuantity(quantity.text)
        if all((getattr(self, a) for a in getValid(self.Supplier))):
            if self.Supplier not in suppliers:
                suppliers.append(self.Supplier)
            return True
        return False


def generateBom(xml, path, quantity):
    suppliers, components, missings = parseXml(xml, quantity)
    writeCsv(suppliers, components, path)
    return suppliers, missings


def parseXml(xml, quantity):
    tree = ElementTree.parse(xml)
    root = tree.getroot()
    suppliers = []
    components = []
    missings = []
    for c in root.findall('./components/'):
        component = Component(c)
        if not component.isvalid:
            missings.append(component.ref)
            continue
        fields = c.find('fields')
        if not component.setCustomFields(fields, suppliers):
            missings.append(component.ref)
            continue
        if component.Quantity == 0:
            missings.append(component.ref)
            continue
        component.Quantity *= quantity
        if needGrouping(component.Supplier):
            exist = next((c for c in components if c == component), None)
            if exist is None:
                components.append(component)
            else:
                exist.Quantity += component.Quantity
        else:
            components.extend([component for c in range(component.Quantity)])
    return sorted(suppliers), sorted(components), missings


def writeCsv(suppliers, components, path):
    for supplier in suppliers:
        out = getOutput(path, supplier, g_bomext)
        columns = getFields(supplier).keys()
        delimiter = getDelimiter(supplier)
        quotechar = getQuotechar(supplier)
        quoting = getQuoting(supplier)
        fields = getFields(supplier).items()
        with open(out, 'w') as csvfile:
            c = csv.DictWriter(csvfile,
                               fieldnames = columns,
                               delimiter = delimiter,
                               quotechar = quotechar,
                               quoting = quoting)
            c.writeheader()
            while(len(components) and components[0].Supplier == supplier):
                row = {}
                component = components.pop(0)
                for key, value in fields:
                    row[key] = getattr(component, value)
                c.writerow(row)


def generateCpl(suppliers, path):
    rewrited = {}
    for supplier in suppliers:
        if needCpl(supplier):
            rewrited[supplier] = False
            for i in getInputs(path):
                if os.path.isfile(i):
                    out = getOutput(path, supplier, g_cplext)
                    headers = getHeaders(supplier)
                    copyCsv(i, out, headers)
                    rewrited[supplier] = True
                    break
    return rewrited


def copyCsv(input, output, headers):
    with open(input) as r, open(output, 'w') as w:
        reader = csv.reader(r)
        writer = csv.writer(w)
        header = next(reader)
        for i, value in headers.items():
           header[i] = value
        writer.writerow(header)
        for row in reader:
            writer.writerow(row)


def getArguments():
    xml = sys.argv[1]
    path = sys.argv[2]
    quantity = 1
    if len(sys.argv) > 3:
        quantity = getQuantity(sys.argv[3], 1)
    return xml, path, quantity


if __name__ == "__main__":
    xml, path, quantity = getArguments()
    suppliers, missings = generateBom(xml, path, quantity)
    print("")
    print(">>> Generating BOM (Bill Of Materials) csv file for (Quantity = %s):" % quantity)
    for supplier in suppliers:
        print("")
        print("%s: %s" % (supplier, getOutput(path, supplier, g_bomext)))

    if len(missings) > 0:
        print("")
        print("WARNING: These componants cannot be integrated:")
        print(", ".join(missings))

    suppliers = generateCpl(suppliers, path)
    if len(suppliers):
        print("")
        print(">>> Generating CPL (Component Placement List) csv file for:")
    for supplier, status in suppliers.items():
        if status:
            print("")
            print("%s: %s:" % (supplier, getOutput(path, supplier, g_cplext)))
        else:
            print("")
            print("ERROR: Can't retrieve a position file like:")
            for i in getInputs(path):
                print(i)
