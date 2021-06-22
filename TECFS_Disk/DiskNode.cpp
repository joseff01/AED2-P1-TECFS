#include "DiskNode.h"

DiskNode::DiskNode(int diskNum) {
    XMLDocument XMLDoc;
    switch (diskNum) {
        case 1: {
            XMLError err = XMLDoc.LoadFile("../../SetupXML/Disk1.xml");
            if (err != XML_SUCCESS) {
                cout << "Error loading file: " << (int) err << endl;
                cout << "Error loading file: " << XMLDoc.ErrorName() << endl;
            }
            break;
        }
        case 2: {
            XMLError err = XMLDoc.LoadFile("../../SetupXML/Disk2.xml");
            if (err != XML_SUCCESS) {
                cout << "Error loading file: " << (int) err << endl;
                cout << "Error loading file: " << XMLDoc.ErrorName() << endl;
            }
            break;
        }
        case 3: {
            XMLError err = XMLDoc.LoadFile("../../SetupXML/Disk3.xml");
            if (err != XML_SUCCESS) {
                cout << "Error loading file: " << (int) err << endl;
                cout << "Error loading file: " << XMLDoc.ErrorName() << endl;
            }
            break;
        }
        case 4: {
            XMLError err = XMLDoc.LoadFile("../../SetupXML/Disk4.xml");
            if (err != XML_SUCCESS) {
                cout << "Error loading file: " << (int) err << endl;
                cout << "Error loading file: " << XMLDoc.ErrorName() << endl;
            }
            break;
        }
    }
    XMLNode *pRoot = XMLDoc.FirstChild();
    XMLElement *pSetupElements = pRoot->FirstChildElement("SetupElements");
    XMLElement *pElement = pSetupElements->FirstChildElement();
    pElement->QueryIntText(&portNo);
    cout << "portNo: "<< portNo << endl;
    pElement = pElement->NextSiblingElement();
    libPath = pElement->GetText();
    cout << "Path: "<<libPath << endl;


}
