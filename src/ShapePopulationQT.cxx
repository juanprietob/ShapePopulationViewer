#include "ShapePopulationQT.h"


ShapePopulationQT::ShapePopulationQT()
{
    this->setupUi(this);

    //Intializations
    m_toolsDisplayed = true;
    m_updateOnPositionChanged = true;
    m_updateOnAttributeChanged = true;
    m_numberOfMeshes = 0;
    m_lastDirectory = "";
    m_colormapDirectory = "";
    m_exportDirectory = "";
    m_cameraDialog = new cameraDialogQT(this);
    m_backgroundDialog = new backgroundDialogQT(this);
    m_CSVloaderDialog = new CSVloaderQT(this);

    // GUI disable
    toolBox->setDisabled(true);
    this->gradientWidget_VISU->disable();
    menuOptions->setDisabled(true);
    actionDelete->setDisabled(true);
    actionDelete_All->setDisabled(true);
    menuExport->setDisabled(true);

    #ifdef SPV_EXTENSION
    menuExport->clear();
    menuExport->addAction("PDF");
    connect(menuExport->actions().at(0),SIGNAL(triggered()),this,SLOT(showNoExportWindow()));
    #endif

    //Pushbuttons color
    pushButton_VISU_add->setStyleSheet("color: rgb(0, 200, 0)");
    pushButton_VISU_delete->setStyleSheet("color: rgb(200, 0, 0)");

    //Checkbox icons
    std::string eyeStyleSheet;
    eyeStyleSheet += "QCheckBox::indicator:checked{ image: url(:/resources/eyeOpen.png);} ";
    eyeStyleSheet += "QCheckBox::indicator:unchecked{ image: url(:/resources/eyeClosed.png);}";
    eyeStyleSheet += "QCheckBox::indicator:checked:hover{ image: url(:/resources/eyeRed.png);} ";
    eyeStyleSheet += "QCheckBox::indicator:checked:pressed{ image: url(:/resources/eyeHalf.png);} ";
    eyeStyleSheet += "QCheckBox::indicator:unchecked:pressed{ image: url(:/resources/eyeHalf.png);}";
    eyeStyleSheet += "QCheckBox::indicator:checked:disabled{ image: url(:/resources/eyeOpenDisabled.png);} ";
    eyeStyleSheet += "QCheckBox::indicator:unchecked:disabled{ image: url(:/resources/eyeClosedDisabled.png);}";

    this->checkBox_displayMeshName->setStyleSheet(QString::fromStdString(eyeStyleSheet));
    this->checkBox_displayAttribute->setStyleSheet(QString::fromStdString(eyeStyleSheet));
    this->checkBox_displayColorbar->setStyleSheet(QString::fromStdString(eyeStyleSheet));

    QPalette backgroundColor = frame_DISPLAY->palette();
    backgroundColor.setColor( backgroundRole(), QColor( 255, 255, 255 ) );
    frame_DISPLAY->setPalette( backgroundColor );

    //Menu signals
    connect(actionExit, SIGNAL(triggered()), this, SLOT(slotExit()));
    connect(actionOpen_Directory,SIGNAL(triggered()),this,SLOT(openDirectory()));
    connect(actionOpen_VTK_Files,SIGNAL(triggered()),this,SLOT(openFiles()));
    connect(actionLoad_CSV,SIGNAL(triggered()),this,SLOT(loadCSV()));
    connect(m_CSVloaderDialog,SIGNAL(sig_itemsSelected(QFileInfoList)),this,SLOT(slot_itemsSelected(QFileInfoList)));
    connect(actionDelete,SIGNAL(triggered()),this,SLOT(deleteSelection()));
    connect(actionDelete_All,SIGNAL(triggered()),this,SLOT(deleteAll()));
    connect(actionCameraConfig,SIGNAL(triggered()),this,SLOT(showCameraConfigWindow()));
    connect(actionBackgroundConfig,SIGNAL(triggered()),this,SLOT(showBackgroundConfigWindow()));
    connect(actionLoad_Colorbar,SIGNAL(triggered()),this,SLOT(loadColorMap()));
    connect(actionSave_Colorbar,SIGNAL(triggered()),this,SLOT(saveColorMap()));
    #ifndef SPV_EXTENSION
    connect(actionTo_PDF,SIGNAL(triggered()),this,SLOT(exportToPDF()));
    connect(actionTo_PS,SIGNAL(triggered()),this,SLOT(exportToPS()));
    connect(actionTo_EPS,SIGNAL(triggered()),this,SLOT(exportToEPS()));
    connect(actionTo_TEX,SIGNAL(triggered()),this,SLOT(exportToTEX()));
    connect(actionTo_SVG,SIGNAL(triggered()),this,SLOT(exportToSVG()));
    #endif
    //gradView Signals
    connect(gradientWidget_VISU,SIGNAL(arrowMovedSignal(qreal)), this, SLOT(slot_gradArrow_moved(qreal)));
    connect(gradientWidget_VISU,SIGNAL(arrowSelectedSignal(qreal)), this, SLOT(slot_gradArrow_selected(qreal)));
    connect(gradientWidget_VISU,SIGNAL(arrowDoubleClickedSignal()), this, SLOT(slot_gradArrow_doubleClicked()));
    connect(gradientWidget_VISU,SIGNAL(noSelectionSignal()), this, SLOT(slot_no_gradArrow_selected()));

    //backgroundDialog signals
    connect(m_backgroundDialog,SIGNAL(sig_selectedColor_valueChanged(QColor)), this, SLOT(slot_selectedColor_valueChanged(QColor)));
    connect(m_backgroundDialog,SIGNAL(sig_unselectedColor_valueChanged(QColor)), this, SLOT(slot_unselectedColor_valueChanged(QColor)));
    connect(m_backgroundDialog,SIGNAL(sig_textColor_valueChanged(QColor)), this, SLOT(slot_textColor_valueChanged(QColor)));

    //cameraDialog signals
    connect(this,SIGNAL(sig_updateCameraConfig(cameraConfigStruct)), m_cameraDialog, SLOT(updateCameraConfig(cameraConfigStruct)));
    connect(m_cameraDialog,SIGNAL(sig_newCameraConfig(cameraConfigStruct)), this, SLOT(slot_newCameraConfig(cameraConfigStruct)));
    connect(m_cameraDialog,SIGNAL(sig_position_x_valueChanged(double)), this, SLOT(slot_position_x_valueChanged(double)));
    connect(m_cameraDialog,SIGNAL(sig_position_y_valueChanged(double)), this, SLOT(slot_position_y_valueChanged(double)));
    connect(m_cameraDialog,SIGNAL(sig_position_z_valueChanged(double)), this, SLOT(slot_position_z_valueChanged(double)));
    connect(m_cameraDialog,SIGNAL(sig_focalpoint_x_valueChanged(double)), this, SLOT(slot_focalpoint_x_valueChanged(double)));
    connect(m_cameraDialog,SIGNAL(sig_focalpoint_y_valueChanged(double)), this, SLOT(slot_focalpoint_y_valueChanged(double)));
    connect(m_cameraDialog,SIGNAL(sig_focalpoint_z_valueChanged(double)), this, SLOT(slot_focalpoint_z_valueChanged(double)));
    connect(m_cameraDialog,SIGNAL(sig_viewup_vx_valueChanged(double)), this, SLOT(slot_viewup_vx_valueChanged(double)));
    connect(m_cameraDialog,SIGNAL(sig_viewup_vy_valueChanged(double)), this, SLOT(slot_viewup_vy_valueChanged(double)));
    connect(m_cameraDialog,SIGNAL(sig_viewup_vz_valueChanged(double)), this, SLOT(slot_viewup_vz_valueChanged(double)));
    connect(m_cameraDialog,SIGNAL(sig_scale_valueChanged(double)), this, SLOT(slot_scale_valueChanged(double)));

    // Data Array
    QStandardItemModel * model = new QStandardItemModel(0,2,this);
    tableView->setModel(model);
    tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableView->horizontalHeader()->setResizeMode(QHeaderView::Fixed);
    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);

    //Display
    radioButton_DISPLAY_all->toggle();                          //Display All surfaces,
    radioButton_SYNC_realtime->toggle();
}


ShapePopulationQT::~ShapePopulationQT()
{
    delete m_cameraDialog;
    delete m_backgroundDialog;
    delete m_CSVloaderDialog;
}

void ShapePopulationQT::slotExit()
{
    this->deleteAll();
    qApp->exit();
}

void ShapePopulationQT::on_pushButton_displayTools_clicked()
{
    if(m_toolsDisplayed)
    {
        this->toolBox->hide();
#ifdef WIN32
          this->pushButton_displayTools->setText(QString("^"));
#else
        this->pushButton_displayTools->setText(QString::fromUtf8("\u25B2"));
#endif
        m_toolsDisplayed = false;
    }
    else
    {
        this->toolBox->show();
#ifdef WIN32
        this->pushButton_displayTools->setText(QString("v"));
#else
        this->pushButton_displayTools->setText(QString::fromUtf8("\u25BC"));
#endif
        m_toolsDisplayed = true;
    }
}

// * ///////////////////////////////////////////////////////////////////////////////////////////// * //
// *                                         CLP FUNCTIONS                                         * //
// * ///////////////////////////////////////////////////////////////////////////////////////////// * //

void ShapePopulationQT::loadVTKFilesCLP(QFileInfoList a_fileList)
{
    //m_fileList.append(file);                      // Add to filelist
    m_fileList.append(a_fileList);
    if(!m_fileList.isEmpty()) this->CreateWidgets();    // Display widgets
}

void ShapePopulationQT::loadCSVFileCLP(QFileInfo file)
{
    //Read .CSV with VTK
    vtkSmartPointer<vtkDelimitedTextReader> CSVreader = vtkSmartPointer<vtkDelimitedTextReader>::New();
    CSVreader->SetFieldDelimiterCharacters(",");
    CSVreader->SetFileName(file.absoluteFilePath().toStdString().c_str());
    CSVreader->SetHaveHeaders(true);
    CSVreader->Update();
    vtkSmartPointer<vtkTable> table = CSVreader->GetOutput();

    //Display in CSVloaderQT
    m_CSVloaderDialog->displayTable(table,file.absoluteDir());
}

void ShapePopulationQT::loadVTKDirCLP(QDir vtkDir)
{
    //Add to fileList
    m_fileList.append(vtkDir.entryInfoList());

    // Control the files format
    for (int i = 0; i < m_fileList.size(); i++)
    {
        QString QFilePath = m_fileList.at(i).canonicalFilePath();
        if (!QFilePath.endsWith(".vtk") && !QFilePath.endsWith(".vtp"))
        {
            m_fileList.removeAt(i);
            i--;
        }
    }

    // Display widgets
    if(!m_fileList.isEmpty()) this->CreateWidgets();
}

void ShapePopulationQT::loadColorMapCLP(std::string a_filePath)
{
    QString QFilePath(a_filePath.c_str());
    gradientWidget_VISU->loadColorPointList(QFilePath, &m_usedColorBar->colorPointList);

    this->updateColorbar_QT();
}

void ShapePopulationQT::loadCameraCLP(std::string a_filePath)
{
    m_cameraDialog->loadCameraConfig(QString(a_filePath.c_str()));
}

// * ///////////////////////////////////////////////////////////////////////////////////////////// * //
// *                                        MENU FUNCTIONS                                         * //
// * ///////////////////////////////////////////////////////////////////////////////////////////// * //

void ShapePopulationQT::openDirectory()
{

    // get directory
    QString dir = QFileDialog::getExistingDirectory(this,tr("Open Directory"),m_lastDirectory,QFileDialog::ShowDirsOnly);
    if(dir.isEmpty() || !QDir(dir).exists()) return;

    // Add files in the fileList
    m_lastDirectory = dir;
    QDir vtkDir(dir);
    m_fileList.append(vtkDir.entryInfoList());

    // Control the files format
    for (int i = 0; i < m_fileList.size(); i++)
    {
        QString QFilePath = m_fileList.at(i).canonicalFilePath();
        if (!QFilePath.endsWith(".vtk") && !QFilePath.endsWith(".vtp"))
        {
            m_fileList.removeAt(i);
            i--;
        }
    }

    // Display widgets
    if(!m_fileList.isEmpty()) this->CreateWidgets();
}


void ShapePopulationQT::openFiles()
{
    QStringList stringList = QFileDialog::getOpenFileNames(this,tr("Open Files"),m_lastDirectory,"VTK Files (*.vtk *.vtp)");
    if(stringList.isEmpty())
    {
        return ;
    }

    m_lastDirectory=QFileInfo(stringList.at(0)).path();

    for(int i=0; i < stringList.size(); i++)
    {
        if(QFileInfo(stringList.at(i)).exists())
            this->m_fileList.append(QFileInfo(stringList.at(i)));
    }

    // Display widgets
    if(!m_fileList.isEmpty())
    {
        this->CreateWidgets();
    }
}


void ShapePopulationQT::loadCSV()
{
    // get directory
    QString filename = QFileDialog::getOpenFileName(this,tr("Open .csv file"),m_lastDirectory,"CSV file (*.csv)");
    if(filename.isEmpty() || !QFileInfo(filename).exists()) return;

    //MAJ lastDirectory
    QFileInfo file(filename);
    m_lastDirectory= file.path();

    //Read .CSV with VTK
    vtkSmartPointer<vtkDelimitedTextReader> CSVreader = vtkSmartPointer<vtkDelimitedTextReader>::New();
    CSVreader->SetFieldDelimiterCharacters(",");
    CSVreader->SetFileName(filename.toStdString().c_str());
    CSVreader->SetHaveHeaders(true);
    CSVreader->Update();
    vtkSmartPointer<vtkTable> table = CSVreader->GetOutput();

    //Display in CSVloaderQT
    m_CSVloaderDialog->displayTable(table,file.absoluteDir());

}


void ShapePopulationQT::slot_itemsSelected(QFileInfoList fileList)
{
    // Add files from CSV loader
    for (int i = 0; i < fileList.size(); i++)
    {
        m_fileList.append(fileList[i]);
    }

    // Display widgets
    if(!m_fileList.isEmpty()) this->CreateWidgets();
}

void ShapePopulationQT::deleteAll()
{
    //clear any Content from the layout
    QGridLayout *Qlayout = (QGridLayout *)this->scrollAreaWidgetContents->layout();
    for (unsigned int i = 0; i < m_widgetList.size(); i++)
    {
        Qlayout->removeWidget(m_widgetList.at(i));
        delete m_widgetList.at(i);
        delete m_meshList.at(i);
    }

    //Disable buttons
    toolBox->setDisabled(true);
    gradientWidget_VISU->disable();
    actionDelete_All->setDisabled(true);
    actionDelete->setDisabled(true);
    menuExport->setDisabled(true);
    menuOptions->setDisabled(true);

    //Initialize Menu actions
    actionOpen_Directory->setText("Open Directory");
    actionOpen_VTK_Files->setText("Open VTK Files");
    actionLoad_CSV->setText("Load CSV File");

    //Empty the meshes FileInfo List
    m_fileList.clear();
    m_meshList.clear();
    m_glyphList.clear();
    m_selectedIndex.clear();
    m_windowsList.clear();
    m_widgetList.clear();
    m_numberOfMeshes = 0;
}


void ShapePopulationQT::deleteSelection()
{
    if(m_selectedIndex.size() == 0) return;

    this->scrollArea->setVisible(false);

    // Deleting the selection, the widget, and the data
    QGridLayout *Qlayout = (QGridLayout *)this->scrollAreaWidgetContents->layout();

    for (unsigned int i = 0; i < m_selectedIndex.size(); i++)
    {
        for(unsigned int j = 0; j < m_widgetList.size(); j++)\
        {
            if( j == m_selectedIndex[i])
            {
                m_fileList.removeAt(j);

                delete m_meshList.at(j);
                m_meshList.erase(m_meshList.begin()+j);
                m_glyphList.erase(m_glyphList.begin()+j);

                m_selectedIndex.erase(m_selectedIndex.begin()+i);           // CAREFUL : erase i value not j value, different vector here
                for(unsigned int k = 0; k < m_selectedIndex.size() ; k++)
                {
                    if (m_selectedIndex[k] > i)  m_selectedIndex[k]-- ;     // and then decrement the upper indeces
                }

                m_windowsList.erase(m_windowsList.begin()+j);

                Qlayout->removeWidget(m_widgetList.at(j));
                delete m_widgetList.at(j);
                m_widgetList.erase(m_widgetList.begin()+j);

                i--;
                break;
            }
        }
    }
    m_numberOfMeshes = m_fileList.size();
    spinBox_DISPLAY_columns->setMaximum(m_numberOfMeshes);

    // If no more widgets, do as deleteAll
    if(m_numberOfMeshes == 0)
    {
        deleteAll();
    }
    else
    {
        ShapePopulationBase::SelectAll();

        computeCommonAttributes();                                                  // get the common attributes in m_commonAttributes
        comboBox_VISU_attribute->clear();                                           // clear the Attributes in the comboBox
        m_colorBarList.clear();                                                     // clear the existing colorbars

        m_updateOnAttributeChanged = false;
        for(unsigned int i = 0 ; i < m_commonAttributes.size() ; i++)
        {
            colorBarStruct * colorBar = new colorBarStruct;                         //new colorbar for this attribute
            gradientWidget_VISU->reset();                                           //create points
            gradientWidget_VISU->getAllColors(&colorBar->colorPointList);           //get the points into the colorbar
            this->UpdateAttribute(m_commonAttributes[i].c_str(), m_selectedIndex);  //create the range
            colorBar->range[0] = m_commonRange[0];                                  //get the range into the colorbar
            colorBar->range[1] = m_commonRange[1];
            m_colorBarList.push_back(colorBar);                                     //add the colorbar to the list

            comboBox_VISU_attribute->addItem(QString(m_commonAttributes[i].c_str()));   // Then add the attribute to the comboBox
        }
        m_updateOnAttributeChanged = true;

        this->UpdateAttribute(m_commonAttributes[0].c_str(), m_selectedIndex);
        m_usedColorBar = m_colorBarList[0];
        this->gradientWidget_VISU->setAllColors(&m_usedColorBar->colorPointList);
        spinBox_VISU_min->setValue(m_usedColorBar->range[0]);
        spinBox_VISU_max->setValue(m_usedColorBar->range[1]);
        this->updateColorbar_QT();
        this->updateArrowPosition();

        this->updateInfo_QT();
        on_spinBox_DISPLAY_columns_editingFinished();
    }
}




// * ///////////////////////////////////////////////////////////////////////////////////////////// * //
// *                                     BACKGROUND FUNCTIONS                                      * //
// * ///////////////////////////////////////////////////////////////////////////////////////////// * //

void ShapePopulationQT::showBackgroundConfigWindow()
{
    m_backgroundDialog->show();
}

void ShapePopulationQT::slot_selectedColor_valueChanged(QColor color)
{
    double selectedColor[3];

    //Colors conversion from 0-255 to 0-1
    selectedColor[0] = (double)color.red()/255.0;
    selectedColor[1] = (double)color.green()/255.0;
    selectedColor[2] = (double)color.blue()/255.0;

    this->setBackgroundSelectedColor(selectedColor);
}


void ShapePopulationQT::slot_unselectedColor_valueChanged(QColor color)
{
    double unselectedColor[3];

    //Colors conversion from 0-255 to 0-1
    unselectedColor[0] = (double)color.red()/255.0;
    unselectedColor[1] = (double)color.green()/255.0;
    unselectedColor[2] = (double)color.blue()/255.0;

    this->setBackgroundUnselectedColor(unselectedColor);
}


void ShapePopulationQT::slot_textColor_valueChanged(QColor color)
{
    double textColor[3];

    //Colors conversion from 0-255 to 0-1
    textColor[0] = (double)color.red()/255.0;
    textColor[1] = (double)color.green()/255.0;
    textColor[2] = (double)color.blue()/255.0;

    this->setLabelColor(textColor);

}

// * ///////////////////////////////////////////////////////////////////////////////////////////// * //
// *                                       CAMERA FUNCTIONS                                        * //
// * ///////////////////////////////////////////////////////////////////////////////////////////// * //

void ShapePopulationQT::showCameraConfigWindow()
{
    m_cameraDialog->show();
    this->UpdateCameraConfig();
}


void ShapePopulationQT::UpdateCameraConfig()
{
    if(m_cameraDialog->isVisible())
    {
        ShapePopulationBase::UpdateCameraConfig();
        emit sig_updateCameraConfig(m_headcamConfig);
    }
}

void ShapePopulationQT::slot_position_x_valueChanged(double arg1)
{
    double * position = m_headcam->GetPosition();
    m_headcam->SetPosition(arg1,position[1],position[2]);
    m_renderAllSelection = true;
    this->RenderSelection();
    m_renderAllSelection = false;
}
void ShapePopulationQT::slot_position_y_valueChanged(double arg1)
{
    double * position = m_headcam->GetPosition();
    m_headcam->SetPosition(position[0],arg1,position[2]);
    m_renderAllSelection = true;
    this->RenderSelection();
    m_renderAllSelection = false;
}
void ShapePopulationQT::slot_position_z_valueChanged(double arg1)
{
    double * position = m_headcam->GetPosition();
    m_headcam->SetPosition(position[0],position[1],arg1);
    m_renderAllSelection = true;
    this->RenderSelection();
    m_renderAllSelection = false;
}
void ShapePopulationQT::slot_focalpoint_x_valueChanged(double arg1)
{
    double * focalpoint = m_headcam->GetFocalPoint();
    m_headcam->SetFocalPoint(arg1,focalpoint[1],focalpoint[2]);
    m_renderAllSelection = true;
    this->RenderSelection();
    m_renderAllSelection = false;
}
void ShapePopulationQT::slot_focalpoint_y_valueChanged(double arg1)
{
    double * focalpoint = m_headcam->GetFocalPoint();
    m_headcam->SetFocalPoint(focalpoint[0],arg1,focalpoint[2]);
    m_renderAllSelection = true;
    this->RenderSelection();
    m_renderAllSelection = false;
}
void ShapePopulationQT::slot_focalpoint_z_valueChanged(double arg1)
{
    double * focalpoint = m_headcam->GetFocalPoint();
    m_headcam->SetFocalPoint(focalpoint[0],focalpoint[1],arg1);
    m_renderAllSelection = true;
    this->RenderSelection();
    m_renderAllSelection = false;
}
void ShapePopulationQT::slot_viewup_vx_valueChanged(double arg1)
{
    double * viewup = m_headcam->GetViewUp();
    m_headcam->SetViewUp(arg1,viewup[1],viewup[2]);
    m_renderAllSelection = true;
    this->RenderSelection();
    m_renderAllSelection = false;
}
void ShapePopulationQT::slot_viewup_vy_valueChanged(double arg1)
{
    double * viewup = m_headcam->GetViewUp();
    m_headcam->SetViewUp(viewup[0],arg1,viewup[2]);
    m_renderAllSelection = true;
    this->RenderSelection();
    m_renderAllSelection = false;
}
void ShapePopulationQT::slot_viewup_vz_valueChanged(double arg1)
{
    double * viewup = m_headcam->GetViewUp();
    m_headcam->SetViewUp(viewup[0],viewup[1],arg1);
    m_renderAllSelection = true;
    this->RenderSelection();
    m_renderAllSelection = false;
}
void ShapePopulationQT::slot_scale_valueChanged(double arg1)
{
    m_headcam->SetParallelScale(arg1);
    m_renderAllSelection = true;
    this->RenderSelection();
    m_renderAllSelection = false;
}

void ShapePopulationQT::slot_newCameraConfig(cameraConfigStruct cam)
{
    m_headcam->SetPosition(cam.pos_x,cam.pos_y,cam.pos_z);
    m_headcam->SetFocalPoint(cam.foc_x,cam.foc_y,cam.foc_z);
    m_headcam->SetViewUp(cam.view_vx,cam.view_vy,cam.view_vz);
    m_headcam->SetParallelScale(cam.scale);
    m_renderAllSelection = true;
    this->RenderSelection();
    m_renderAllSelection = false;
}

// * ///////////////////////////////////////////////////////////////////////////////////////////// * //
// *                                       LOAD/SAVE COLORBAR                                      * //
// * ///////////////////////////////////////////////////////////////////////////////////////////// * //

void ShapePopulationQT::loadColorMap()
{
    QString filename = QFileDialog::getOpenFileName(this,tr("Open .spvcm file"),m_colormapDirectory,"SPVCM file (*.spvcm)");
    if(filename == "") return;

    QFileInfo file(filename);
    m_colormapDirectory= file.path();
    gradientWidget_VISU->loadColorPointList(filename, &m_usedColorBar->colorPointList);

    this->updateColorbar_QT();
}


void ShapePopulationQT::saveColorMap()
{
    QString filename = QFileDialog::getSaveFileName(this,tr("Save .spvcm file"),m_colormapDirectory,"SPVCM file (*.spvcm)");
    if(filename == "") return;

    QFileInfo file(filename);
    m_colormapDirectory= file.path();
    if(file.suffix() != QString("spvcm")) filename += ".spvcm";
    gradientWidget_VISU->saveColorPointList(filename);
}


// * ///////////////////////////////////////////////////////////////////////////////////////////// * //
// *                                       CREATE WIDGETS                                          * //
// * ///////////////////////////////////////////////////////////////////////////////////////////// * //

void ShapePopulationQT::CreateWidgets()
{
    this->scrollArea->setVisible(false);

    /* VTK WINDOWS */
    for (int i = m_numberOfMeshes; i < m_fileList.size(); i++)
    {
        //get filepath and fileNames
        QByteArray path = m_fileList[i].absoluteFilePath().toLatin1();
        const char *filePath = path.data();

        CreateNewWindow(filePath);
    }

    /* QT WIDGETS */
    for (int i = m_numberOfMeshes; i < m_fileList.size(); i++)
    {
        QVTKWidget *meshWidget = new QVTKWidget(this->scrollAreaWidgetContents);
        m_widgetList.push_back(meshWidget);
        meshWidget->GetRenderWindow()->AddRenderer(m_windowsList.at(i)->GetRenderers()->GetFirstRenderer());
        meshWidget->GetInteractor()->AddObserver(vtkCommand::LeftButtonPressEvent, this, &ShapePopulationQT::ClickEvent);
        meshWidget->GetInteractor()->AddObserver(vtkCommand::KeyPressEvent, this, &ShapePopulationBase::KeyPressEventVTK);
        meshWidget->GetInteractor()->AddObserver(vtkCommand::ModifiedEvent, this, &ShapePopulationBase::CameraChangedEventVTK);
        meshWidget->GetInteractor()->AddObserver(vtkCommand::StartInteractionEvent, this, &ShapePopulationBase::StartEventVTK);
        meshWidget->GetInteractor()->AddObserver(vtkCommand::EndInteractionEvent, this, &ShapePopulationBase::EndEventVTK);
    }


    /* WINDOWS */
    m_windowsList.clear();
    for (unsigned int i = 0; i < m_widgetList.size(); i++)
    {
        m_windowsList.push_back(m_widgetList.at(i)->GetRenderWindow());
    }
    RealTimeRenderSynchro(radioButton_SYNC_realtime->isChecked());              //Start with a realtime synchro

    /* ATTRIBUTES & COLORBARS */
    ShapePopulationBase::SelectAll();

    computeCommonAttributes();                                                  // get the common attributes in m_commonAttributes
    comboBox_VISU_attribute->clear();                                           // clear the Attributes in the comboBox
    m_colorBarList.clear();                                                     // clear the existing colorbars

    m_updateOnAttributeChanged = false;
    for(unsigned int i = 0 ; i < m_commonAttributes.size() ; i++)
    {
        colorBarStruct * colorBar = new colorBarStruct;                         //new colorbar for this attribute
        gradientWidget_VISU->reset();                                           //create points
        gradientWidget_VISU->getAllColors(&colorBar->colorPointList);           //get the points into the colorbar
        this->UpdateAttribute(m_commonAttributes[i].c_str(), m_selectedIndex);  //create the range
        colorBar->range[0] = m_commonRange[0];                                  //get the range into the colorbar
        colorBar->range[1] = m_commonRange[1];
        m_colorBarList.push_back(colorBar);                                     //add the colorbar to the list

        comboBox_VISU_attribute->addItem(QString(m_commonAttributes[i].c_str()));   // Then add the attribute to the comboBox
    }
    m_updateOnAttributeChanged = true;

    /* RENDER WINDOWS */
    this->UpdateAttribute(m_commonAttributes[0].c_str(), m_selectedIndex);
    m_usedColorBar = m_colorBarList[0];
    this->gradientWidget_VISU->setAllColors(&m_usedColorBar->colorPointList);
    spinBox_VISU_min->setValue(m_usedColorBar->range[0]);
    spinBox_VISU_max->setValue(m_usedColorBar->range[1]);
    this->updateColorbar_QT();
    this->updateArrowPosition();

    /* VECTORS UPDATE */
    this->setMeshOpacity((double)this->spinbox_meshOpacity->value()/100.0);
    this->setVectorScale((double)this->spinbox_vectorScale->value()/100.0);
    this->setVectorDensity(this->spinbox_arrowDens->value());

    m_numberOfMeshes = m_fileList.size();
    spinBox_DISPLAY_columns->setMaximum(m_numberOfMeshes);

    /* CHECK ALIGNMENT */
    on_comboBox_alignment_currentIndexChanged();

    /* GUI BUTTONS & ACTIONS */
    this->toolBox->setEnabled(true);
    this->gradientWidget_VISU->enable(&m_usedColorBar->colorPointList);
    this->menuOptions->setEnabled(true);
    this->actionDelete->setEnabled(true);
    this->actionDelete_All->setEnabled(true);
    this->menuExport->setEnabled(true);
    this->actionOpen_Directory->setText("Add Directory");
    this->actionOpen_VTK_Files->setText("Add VTK files");
    this->actionLoad_CSV->setText("Add CSV file");\

    /* DISPLAY INFOS */
    this->updateInfo_QT();

    /* GUI WIDGETS DISPLAY */
    unsigned int sum = 0;
    int colNumber = 0;
    int nextOdd = 1;
    for ( ; sum < m_numberOfMeshes ; colNumber++ , nextOdd += 2)
    {
        sum += nextOdd;                                         //simple integer square root, will give the ceiling of the colNumber => cols >= rows
    }
    spinBox_DISPLAY_columns->setValue(colNumber+1);             //Display the number of columns in spinBox_DISPLAY_columns,
    on_spinBox_DISPLAY_columns_editingFinished();                  //and display the Widgets according to this number.

}

// * ///////////////////////////////////////////////////////////////////////////////////////////// * //
// *                                          SELECTION                                            * //
// * ///////////////////////////////////////////////////////////////////////////////////////////// * //

void ShapePopulationQT::ClickEvent(vtkObject* a_selectedObject, unsigned long notUseduLong, void* notUsedVoid)
{

    //Get the interactor used
    vtkSmartPointer<QVTKInteractor> selectedInteractor = vtkSmartPointer<QVTKInteractor>::New();
    selectedInteractor = (QVTKInteractor*)a_selectedObject;
    vtkSmartPointer<vtkRenderWindow> selectedWindow = selectedInteractor->GetRenderWindow();
    unsigned int index = getSelectedIndex(selectedWindow);

    //if the renderwindow already is in the renderselectedWindows...
    if( (std::find(m_selectedIndex.begin(), m_selectedIndex.end(), index)) != (m_selectedIndex.end()) )
    {
        if(selectedInteractor->GetControlKey()==0) return; // ...and is not being unselected : quit
    }

    /* VTK SELECTION */
    ShapePopulationBase::ClickEvent(a_selectedObject,notUseduLong,notUsedVoid);


    if(m_selectedIndex.size() == 0)
    {
        /* DISABLE GUI ACTIONS */
        this->actionDelete->setDisabled(true);
        this->menuExport->setDisabled(true);
        this->groupBox_VIEW->setDisabled(true);
        this->groupBox_VISU->setDisabled(true);
        this->gradientWidget_VISU->disable();
        this->tabWidget->setDisabled(true);
    }
    else
    {
        /* ENABLE GUI ACTIONS */
        this->actionDelete->setEnabled(true);
        this->menuExport->setEnabled(true);
        this->groupBox_VIEW->setEnabled(true);
        this->groupBox_VISU->setEnabled(true);
        this->gradientWidget_VISU->enable(&m_usedColorBar->colorPointList);
        this->tabWidget->setEnabled(true);

        /* DISPLAY INFOS */
        this->updateInfo_QT();
        this->updateAttribute_QT();
        this->setVectorDensity(this->spinbox_arrowDens->value());
    }
}

void ShapePopulationQT::SelectAll()
{
    // if everything already selected
    if(m_selectedIndex.size() == m_windowsList.size()) return;

    ShapePopulationBase::SelectAll();


    /* ENABLE GUI ACTIONS */
    this->actionDelete->setEnabled(true);
    this->menuExport->setEnabled(true);
    this->groupBox_VIEW->setEnabled(true);
    this->groupBox_VISU->setEnabled(true);
    this->gradientWidget_VISU->enable(&m_usedColorBar->colorPointList);
    this->tabWidget->setEnabled(true);

    /* UPDATE WINDOWS */

    // Get Attribute in ComboBox
    QString text = this->comboBox_VISU_attribute->currentText();
    QByteArray arr = text.toLatin1();
    const char *cmap  = arr.data();

    // Update color with this attribute
    this->UpdateAttribute(cmap, m_selectedIndex);
    this->setVectorDensity(this->spinbox_arrowDens->value());
    this->UpdateColorMap(m_selectedIndex);

    // Render
    m_renderAllSelection = true;
    this->RenderSelection();
    m_renderAllSelection = false;

    /* DISPLAY INFOS */
    this->updateInfo_QT();
}


void ShapePopulationQT::UnselectAll()
{
    // if nothing already selected
    if(m_selectedIndex.size() == 0) return;

    ShapePopulationBase::UnselectAll();

    /* DISABLE GUI ACTIONS */
    this->actionDelete->setDisabled(true);
    this->menuExport->setDisabled(true);
    this->groupBox_VIEW->setDisabled(true);
    this->groupBox_VISU->setDisabled(true);
    this->gradientWidget_VISU->disable();
    this->tabWidget->setDisabled(true);
}

void ShapePopulationQT::keyPressEvent(QKeyEvent * keyEvent)
{
   //UNSELECTING
   if((keyEvent->key() == Qt::Key_Escape))
   {
       UnselectAll();
   }
}

// * ///////////////////////////////////////////////////////////////////////////////////////////// * //
// *                                       PLACING FUNCTIONS                                       * //
// * ///////////////////////////////////////////////////////////////////////////////////////////// * //

int ShapePopulationQT::getNumberOfColumns()
{
    QString QStr_colNumber = this->spinBox_DISPLAY_columns->text();
    unsigned int colNumber = QStr_colNumber.toInt();

    if(colNumber > m_numberOfMeshes)
    {
        colNumber = m_numberOfMeshes;
        spinBox_DISPLAY_columns->setValue(colNumber);
    }
    else if(colNumber <= 0)
    {
        colNumber = 1;
        spinBox_DISPLAY_columns->setValue(colNumber);
    }
    return colNumber;
}


int ShapePopulationQT::getNumberOfRows(unsigned int colNumber)
{
    unsigned int rowNumber = m_numberOfMeshes/colNumber;
    if (m_numberOfMeshes%colNumber != 0)
    {
        rowNumber++;
    }
    return rowNumber;
}


void ShapePopulationQT::placeWidgetInArea(unsigned int colNumber)
{
    unsigned int i_col = 0;
    unsigned int i_row = 0;

    for (unsigned int i = 0; i < m_numberOfMeshes ;i++)
    {
        QGridLayout *Qlayout = (QGridLayout *)this->scrollAreaWidgetContents->layout();
        Qlayout->addWidget(m_widgetList.at(i),i_row,i_col);

        if (i_col == colNumber-1)
        {
            i_col = 0;
            i_row++;
        }
        else i_col++;
    }
}


void ShapePopulationQT::resizeWidgetInArea()
{
    if(m_numberOfMeshes == 0) return;

    QSize QSize_scrollArea = this->scrollAreaWidgetContents->size();
    int scrollAreaWidth = QSize_scrollArea.width();

    int colNumber = getNumberOfColumns();
    int rowNumber = getNumberOfRows(colNumber);
    this->scrollAreaWidgetContents->resize(scrollAreaWidth,(scrollAreaWidth)*rowNumber/colNumber);
}


void ShapePopulationQT::on_tabWidget_currentChanged(int index)
{
    if(index == 1)
    {
        int tableWidth = tableView->width();
        int column0Width = tableView->columnWidth(0);
        int column1Width = tableView->columnWidth(1);
        int column2width = tableWidth - column0Width - column1Width;

        if(m_commonAttributes.size()<6) tableView->setColumnWidth(2,column2width -5);
        else tableView->setColumnWidth(2,column2width-20);
    }
}

void ShapePopulationQT::resizeEvent(QResizeEvent *Qevent)
{
    //Resizing Windows
    QMainWindow::resizeEvent(Qevent);

    //According to the View Options
    if (this->radioButton_DISPLAY_square->isChecked() == true )//view square meshes
    {
        resizeWidgetInArea();
    }

    //data range column
    on_tabWidget_currentChanged(1);
}

void ShapePopulationQT::dragEnterEvent(QDragEnterEvent *Qevent)
{
    Qevent->accept();
}

void ShapePopulationQT::dropEvent(QDropEvent* Qevent)
{
    const QMimeData* mimeData = Qevent->mimeData();

    // check for our needed mime type, here a file or a list of files
    if (mimeData->hasUrls())
    {
        QList<QUrl> urlList = mimeData->urls();
        bool load = false;
        QFileInfoList fileList;

        // extract the local paths of the files
        for (int i = 0; i < urlList.size(); ++i)
        {
            QString filePath = urlList.at(i).toLocalFile();
            if((filePath.endsWith(".vtk") || filePath.endsWith(".vtp")) && QFileInfo(filePath).exists())
            {
                fileList.append(QFileInfo(filePath));
                load = true;
            }
            else if(filePath.endsWith(".csv") && QFileInfo(filePath).exists())
            {
                this->loadCSVFileCLP(QFileInfo(filePath));
            }
            else if(filePath.endsWith(".spvcm") && m_numberOfMeshes > 0 && QFileInfo(filePath).exists())
            {
                loadColorMapCLP(filePath.toStdString());
            }
            else if(filePath.endsWith(".pvcc") && m_numberOfMeshes > 0 && QFileInfo(filePath).exists())
            {
                loadCameraCLP(filePath.toStdString());
            }
            else if(QDir(filePath).exists())
            {
                this->loadVTKDirCLP(QDir(filePath));
            }
        }
        if(load == true)
        {
            this->loadVTKFilesCLP(fileList);
        }
    }
}

// * ///////////////////////////////////////////////////////////////////////////////////////////// * //
// *                                       DISPLAY OPTIONS                                         * //
// * ///////////////////////////////////////////////////////////////////////////////////////////// * //


void ShapePopulationQT::on_radioButton_DISPLAY_all_toggled()
{
    this->scrollArea->setWidgetResizable(true);
}


void ShapePopulationQT::on_radioButton_DISPLAY_square_toggled()
{
    this->scrollArea->setWidgetResizable(false);
    resizeWidgetInArea();
}

void ShapePopulationQT::on_spinBox_DISPLAY_columns_editingFinished()
{
    if(m_numberOfMeshes == 0) return;
    this->scrollArea->setVisible(false);

    placeWidgetInArea(spinBox_DISPLAY_columns->value());
    if (radioButton_DISPLAY_square->isChecked()) resizeWidgetInArea();

    this->scrollArea->setVisible(true);
}

void ShapePopulationQT::on_checkBox_displayColorbar_toggled(bool checked)
{
    this->displayColorbar(checked);
    this->RenderAll();
}

void ShapePopulationQT::on_checkBox_displayAttribute_toggled(bool checked)
{
    this->displayAttribute(checked);
    this->RenderAll();
}

void ShapePopulationQT::on_checkBox_displayMeshName_toggled(bool checked)
{
    this->displayMeshName(checked);
    this->RenderAll();
}

// * ///////////////////////////////////////////////////////////////////////////////////////////// * //
// *                                        SYNCHRONISATION                                        * //
// * ///////////////////////////////////////////////////////////////////////////////////////////// * //


void ShapePopulationQT::on_radioButton_SYNC_realtime_toggled()
{
    RealTimeRenderSynchro(true);
}

void ShapePopulationQT::on_radioButton_SYNC_delayed_toggled()
{
    RealTimeRenderSynchro(false);
}

void ShapePopulationQT::on_pushButton_SYNC_all_clicked()
{
    SelectAll();
}
void ShapePopulationQT::on_pushButton_SYNC_unselect_clicked()
{
    UnselectAll();
}

// * ///////////////////////////////////////////////////////////////////////////////////////////// * //
// *                                         VIEW FUNCTIONS                                        * //
// * ///////////////////////////////////////////////////////////////////////////////////////////// * //

void ShapePopulationQT::on_pushButton_VIEW_reset_clicked()
{    
    if(m_selectedIndex.size() == 0) return;

    this->ResetHeadcam();

    m_renderAllSelection = true;
    this->RenderSelection();
    m_renderAllSelection = false;
}

void ShapePopulationQT::on_toolButton_VIEW_P_clicked() {ChangeView(0,0,-1);}

void ShapePopulationQT::on_toolButton_VIEW_A_clicked() {ChangeView(0,0,1);}

void ShapePopulationQT::on_toolButton_VIEW_L_clicked() {ChangeView(1,0,0);}

void ShapePopulationQT::on_toolButton_VIEW_R_clicked() {ChangeView(-1,0,0);}

void ShapePopulationQT::on_toolButton_VIEW_S_clicked() {ChangeView(0,1,0);}

void ShapePopulationQT::on_toolButton_VIEW_I_clicked() {ChangeView(0,-1,0);}

void ShapePopulationQT::on_comboBox_alignment_currentIndexChanged()
{
    // Get Attribute in ComboBox
    int alignment = this->comboBox_alignment->currentIndex();

    if(alignment == 0) this->AlignMesh(false);
    else if(alignment == 1) this->AlignMesh(true);

    this->RenderAll();
}

// * ///////////////////////////////////////////////////////////////////////////////////////////// * //
// *                                     ATTRIBUTES COLORMAP                                       * //
// * ///////////////////////////////////////////////////////////////////////////////////////////// * //

void ShapePopulationQT::on_comboBox_VISU_attribute_currentIndexChanged()
{
    if(m_selectedIndex.size() == 0 || m_updateOnAttributeChanged == false) return;

    int index = this->comboBox_VISU_attribute->currentIndex();
    if (index != -1)
    {
        // Update Attribute and commonRange
        QString text = this->comboBox_VISU_attribute->currentText();
        QByteArray arr = text.toLatin1();
        const char *cmap  = arr.data();
        this->UpdateAttribute(cmap, m_selectedIndex);

        // Update Vectors
        int dimension = m_meshList[0]->GetPolyData()->GetPointData()->GetScalars(cmap)->GetNumberOfComponents();
        if (dimension == 3)
        {
            this->setVectorDensity(this->spinbox_arrowDens->value());
        }

        // Change the colorbar selected
        m_usedColorBar = m_colorBarList[index]; //the colorbar depends of the attribute
        this->gradientWidget_VISU->setAllColors(&m_usedColorBar->colorPointList);
        spinBox_VISU_min->setValue(m_usedColorBar->range[0]);
        spinBox_VISU_max->setValue(m_usedColorBar->range[1]);

        // Display colormap
        this->UpdateColorMap(m_selectedIndex);
        this->updateArrowPosition();

        // Render
        m_renderAllSelection = true;
        this->RenderSelection();
        m_renderAllSelection = false;
    }
}

void ShapePopulationQT::updateColorbar_QT()
{
    // Update m_colorPointList from colorbar
    gradientWidget_VISU->getAllColors(&m_usedColorBar->colorPointList);

    // Get Attribute in ComboBox
    QString text = this->comboBox_VISU_attribute->currentText();
    QByteArray arr = text.toLatin1();
    const char *cmap  = arr.data();
    std::ostringstream strs;
    strs.str(""); strs.clear();
    strs << cmap << "_mag"<<std::endl;
    std::string cmap_mag = strs.str();

    //Get the windows with this attribute
    std::vector<unsigned int > windowsIndex;
    for(unsigned int i = 0 ; i < m_windowsList.size() ; i++)
    {
        const char * thisCmap = m_meshList[i]->GetPolyData()->GetPointData()->GetScalars()->GetName();

        if(std::string(thisCmap) == std::string(cmap) || std::string(thisCmap) == cmap_mag)
        {
            windowsIndex.push_back(i);
        }
    }

    //Updating this "sameattributewindows" list colormap
    this->UpdateColorMap(windowsIndex);

    //Rendering those windows...
    for(unsigned int i = 0 ; i < windowsIndex.size() ; i++)
    {
        m_windowsList[windowsIndex[i]]->Render();
    }
}

void ShapePopulationQT::updateArrowPosition()
{
    m_updateOnPositionChanged = false;

    // Update Spinbox ranges
    this->spinBox_VISU_position->setMinimum(m_usedColorBar->range[0]);
    this->spinBox_VISU_position->setMaximum(m_usedColorBar->range[1]);
    double range = fabs(m_usedColorBar->range[1] - m_usedColorBar->range[0]);
    this->spinBox_VISU_position->setSingleStep(range/100);

    // Update Spinbox value
    qreal newPos = this->gradientWidget_VISU->getFocusPosition();
    this->slot_gradArrow_moved(newPos);

    m_updateOnPositionChanged = true;
}


void ShapePopulationQT::on_spinBox_VISU_min_editingFinished()
{
    if(spinBox_VISU_min->value() > spinBox_VISU_max->value())
    {
        spinBox_VISU_min->setValue(spinBox_VISU_max->value());
        return;
    }

    m_usedColorBar->range[0] = spinBox_VISU_min->value();
    this->updateColorbar_QT();
    this->updateArrowPosition();
}

void ShapePopulationQT::on_spinBox_VISU_max_editingFinished()
{
    if(spinBox_VISU_max->value() < spinBox_VISU_min->value())
    {
        spinBox_VISU_max->setValue(spinBox_VISU_min->value());
        return;
    }
    m_usedColorBar->range[1] = spinBox_VISU_max->value();
    this->updateColorbar_QT();
    this->updateArrowPosition();
}


void ShapePopulationQT::on_pushButton_VISU_resetRange_clicked()
{
    m_usedColorBar->range[0] = m_commonRange[0];
    m_usedColorBar->range[1] = m_commonRange[1];
    spinBox_VISU_min->setValue(m_usedColorBar->range[0]);
    spinBox_VISU_max->setValue(m_usedColorBar->range[1]);

    this->updateColorbar_QT();
    this->updateArrowPosition();

}

void ShapePopulationQT::on_pushButton_VISU_delete_clicked()
{
    gradientWidget_VISU->deleteFocusArrow();

    if(gradientWidget_VISU->getNumberOfArrows() <= 2) pushButton_VISU_delete->setDisabled(true);
    this->updateColorbar_QT();
}

void ShapePopulationQT::on_spinBox_VISU_position_valueChanged(double arg1)
{
    if (m_colorBarList.size() == 0 || m_updateOnPositionChanged == false) return;

    //get relative position depending on the range
    double range = fabs(m_usedColorBar->range[1] - m_usedColorBar->range[0]);
    qreal newPos = (arg1-m_usedColorBar->range[0])/range;

    //setting the position to the arrow
    if (newPos >= 0.0 && newPos <= 1.0) gradientWidget_VISU->setFocusPosition(newPos);
    else if(newPos < 0.0) gradientWidget_VISU->setFocusPosition(0.0);
    else if(newPos > 1.0) gradientWidget_VISU->setFocusPosition(1.0);

    //update
    this->updateColorbar_QT();
}

void ShapePopulationQT::on_pushButton_VISU_add_clicked()
{
    QColor color;
    color = QColorDialog::getColor();
    if (color.isValid())
    {
        gradientWidget_VISU->addArrow(color, 0.7, true);
        this->updateColorbar_QT();
    }
}

void ShapePopulationQT::on_pushButton_VISU_reset_clicked()
{
    gradientWidget_VISU->reset();
    this->updateColorbar_QT();
}

// * ///////////////////////////////////////////////////////////////////////////////////////////// * //
// *                                         COLOR ARROWS                                          * //
// * ///////////////////////////////////////////////////////////////////////////////////////////// * //

void ShapePopulationQT::slot_gradArrow_moved(qreal newPos)
{
    //Get the absolute position of the arrow
    double range = fabs(m_usedColorBar->range[1] - m_usedColorBar->range[0]);
    double absPos = m_usedColorBar->range[0] + range * newPos;

    //set the spinbox value
    spinBox_VISU_position->setValue(absPos);
}

void ShapePopulationQT::slot_gradArrow_selected(qreal newPos)
{
    if(m_numberOfMeshes == 0) return;

    if(gradientWidget_VISU->getNumberOfArrows() > 2) pushButton_VISU_delete->setEnabled(true);
    spinBox_VISU_position->setEnabled(true);

    this->slot_gradArrow_moved(newPos);
    this->spinBox_VISU_position->setFocus();
    this->spinBox_VISU_position->selectAll();
}

void ShapePopulationQT::slot_gradArrow_doubleClicked()
{
    QColor color = gradientWidget_VISU->getFocusColor();
    color = QColorDialog::getColor(color);
    if (color.isValid())
    {
        gradientWidget_VISU->setFocusColor(color);
    }

    this->updateColorbar_QT();
}

void ShapePopulationQT::slot_no_gradArrow_selected()
{
    pushButton_VISU_delete->setDisabled(true);
    spinBox_VISU_position->setDisabled(true);
}


// * ///////////////////////////////////////////////////////////////////////////////////////////// * //
// *                                             INFO                                              * //
// * ///////////////////////////////////////////////////////////////////////////////////////////// * //
void ShapePopulationQT::updateInfo_QT()
{
    // Data Array Headers
    QStandardItemModel * model = (QStandardItemModel*)tableView->model();
    model->clear();
    model->setHorizontalHeaderItem(0, new QStandardItem(QString("Name")));
    model->setHorizontalHeaderItem(1, new QStandardItem(QString("Dim")));
    model->setHorizontalHeaderItem(2, new QStandardItem(QString("Range")));

	std::ostringstream strs;

    if(m_selectedIndex.size() > 1)
    {
		strs.str(""); strs.clear();
		strs << (int)m_selectedIndex.size()
             <<" surfaces selected, select only one"<< std::endl;

        //Infos
		this->lineEdit_filename->setText(QString(strs.str().c_str()));
        this->lineEdit_dir->setText(QString(""));
        this->lineEdit_points->setText(QString(""));
        this->lineEdit_cells->setText(QString(""));

        // Data Array Values
        for(unsigned int i=0; i < m_commonAttributes.size(); i++)
        {
            //Name
            QStandardItem * dataName = new QStandardItem(QString(m_commonAttributes[i].c_str()));
            model->setItem(i,0,dataName);

            //Dimension
            ShapePopulationData * mesh = m_meshList[0];
            int dim = mesh->GetPolyData()->GetPointData()->GetScalars(m_commonAttributes[i].c_str())->GetNumberOfComponents();
            strs.str(""); strs.clear();
            strs <<dim;
            QStandardItem * dimension = new QStandardItem(QString(strs.str().c_str()));
            model->setItem(i,1,dimension);

            //Range
            double * range = m_commonRange;
            if(dim == 1)
            {
                range = computeCommonRange(m_commonAttributes[i].c_str(), m_selectedIndex);
            }
            else if(dim == 3)
            {
                strs.str(""); strs.clear();
                strs <<m_commonAttributes[i]<<"_mag"<<std::endl;
                range = computeCommonRange(strs.str().c_str(), m_selectedIndex);
            }
			strs.str(""); strs.clear();
            strs <<"[ "<<range[0]<<" ; "<<range[1]<<" ]";
            QStandardItem * dataRange = new QStandardItem(QString(strs.str().c_str()));
            model->setItem(i,2,dataRange);
        }
    }
    else
    {

        //Infos
        unsigned int index = m_selectedIndex[0];
        vtkSmartPointer<vtkPolyData> selectedData = m_meshList[index]->GetPolyData();

        this->lineEdit_filename->setText(QString(m_meshList[index]->GetFileName().c_str()));
        this->lineEdit_dir->setText(QString(m_meshList[index]->GetFileDir().c_str()));
		strs.str(""); strs.clear();
        strs << (int)selectedData->GetNumberOfPoints();
		this->lineEdit_points->setText(QString(strs.str().c_str()));
		strs.str(""); strs.clear();
        strs << (int)selectedData->GetNumberOfCells();
        this->lineEdit_cells->setText(QString(strs.str().c_str()));


        // Data Array Values
        std::vector<std::string> AttributesList = m_meshList[index]->GetAttributeList();

        for(unsigned int i=0; i < AttributesList.size(); i++)
        {
            //Name
            QStandardItem * dataName = new QStandardItem(QString(AttributesList[i].c_str()));
            model->setItem(i,0,dataName);

            //Dimension
            ShapePopulationData * mesh = m_meshList[index];
            int dim = mesh->GetPolyData()->GetPointData()->GetScalars(AttributesList[i].c_str())->GetNumberOfComponents();
            strs.str(""); strs.clear();
            strs <<dim;
            QStandardItem * dimension = new QStandardItem(QString(strs.str().c_str()));
            model->setItem(i,1,dimension);

            //Range
            double * range = m_commonRange;
            if(dim == 1)
            {
                range = computeCommonRange(AttributesList[i].c_str(), m_selectedIndex);
            }
            else if(dim == 3)
            {
                strs.str(""); strs.clear();
                strs <<AttributesList[i]<<"_mag"<<std::endl;
                range = computeCommonRange(strs.str().c_str(), m_selectedIndex);
            }
			strs.str(""); strs.clear();
            strs <<"[ "<<range[0]<<" ; "<<range[1]<<" ]";
            QStandardItem * dataRange = new QStandardItem(QString(strs.str().c_str()));
            model->setItem(i,2,dataRange);
        }
    }

    /* Adapt Columns size */
    tableView->resizeColumnToContents(0);
    tableView->resizeColumnToContents(1);
    on_tabWidget_currentChanged(1);
}

void ShapePopulationQT::updateAttribute_QT()
{
    if(m_selectedIndex.size() == 1)  // if new selection (TODO : wrong if unselect one and one left)
    {
        const char * cmap = m_meshList[m_selectedIndex[0]]->GetPolyData()->GetPointData()->GetScalars()->GetName();
        int index = comboBox_VISU_attribute->findText(cmap);

        if (index !=comboBox_VISU_attribute->currentIndex() && index != -1) // 1. different attribute (scalar)
        {
            comboBox_VISU_attribute->setCurrentIndex(index);
        }
        else if (index == comboBox_VISU_attribute->currentIndex())  // 2. same attribute but needs to update range
        {
            double * commonRange = computeCommonRange(cmap, m_selectedIndex);
            m_commonRange[0] = commonRange[0];
            m_commonRange[1] = commonRange[1];
        }
        else // (if the scalar attribute cmap is a _mag one, it's vectors)
        {
            const char * cmap_vector = m_meshList[m_selectedIndex[0]]->GetPolyData()->GetPointData()->GetVectors()->GetName();
            int index_vector = comboBox_VISU_attribute->findText(cmap_vector);

            if (index_vector !=comboBox_VISU_attribute->currentIndex() && index_vector != -1) // 3. different attribute (vector)
            {
                comboBox_VISU_attribute->setCurrentIndex(index_vector);
            }
            else if (index_vector == comboBox_VISU_attribute->currentIndex()) // 4. same attribute but need to update range (magnitude)
            {
                double * commonRange = computeCommonRange(cmap, m_selectedIndex);
                m_commonRange[0] = commonRange[0];
                m_commonRange[1] = commonRange[1];
            }
        }

    }
}


// * ///////////////////////////////////////////////////////////////////////////////////////////// * //
// *                                           VECTORS                                             * //
// * ///////////////////////////////////////////////////////////////////////////////////////////// * //

void ShapePopulationQT::on_slider_vectorScale_valueChanged(int value)
{
    double val = (double)value/100;
    this->setVectorScale(val);
    this->RenderAll();
}

void ShapePopulationQT::on_slider_meshOpacity_valueChanged(int value)
{
    double val = (double)value/100;
    this->setMeshOpacity(val);
    this->RenderAll();
}

void ShapePopulationQT::on_slider_arrowDens_valueChanged(int value)
{
    this->setVectorDensity(value);
    this->RenderAll();
}

void ShapePopulationQT::on_checkBox_displayVectors_toggled(bool checked)
{
    this->displayVectors(checked);
    this->RenderAll();
}


// * ///////////////////////////////////////////////////////////////////////////////////////////// * //
// *                                            EXPORT                                             * //
// * ///////////////////////////////////////////////////////////////////////////////////////////// * //
#ifndef SPV_EXTENSION
void ShapePopulationQT::exportToPS()
{
    if (this->getExportDirectory() == 0) return;
    this->exportTo(0);
}
void ShapePopulationQT::exportToEPS()
{
    if (this->getExportDirectory() == 0) return;
    this->exportTo(1);
}
void ShapePopulationQT::exportToPDF()
{
    if (this->getExportDirectory() == 0) return;
    this->exportTo(2);
}
void ShapePopulationQT::exportToTEX()
{
    if (this->getExportDirectory() == 0) return;
    this->exportTo(3);
}
void ShapePopulationQT::exportToSVG()
{
    if (this->getExportDirectory() == 0) return;
    this->exportTo(4);
}

int ShapePopulationQT::getExportDirectory()
{
    QFileDialog dirWindow;
    QString dir = dirWindow.getExistingDirectory(this,tr("Save to Directory"),m_exportDirectory);
    if(dir.isEmpty()) return 0;

    m_exportDirectory= dir;
    return 1;
}

void ShapePopulationQT::exportTo(int fileFormat)
{
    vtkGL2PSExporter * exporter = vtkGL2PSExporter::New();
    exporter->SetFileFormat(fileFormat); //see vtkGL2PSExporter::OutputFormat
    exporter->CompressOff();

    for (unsigned int i = 0; i < m_selectedIndex.size(); i++)
    {
        ShapePopulationData * mesh = m_meshList[m_selectedIndex[i]];
        QFileInfo meshfile(mesh->GetFileName().c_str());
        QString meshName = meshfile.baseName();
        QString meshAttribute(mesh->GetPolyData()->GetPointData()->GetScalars()->GetName());
        QString filePrefix = m_exportDirectory + "/" + meshName + "_" + meshAttribute;

        exporter->SetInput(m_windowsList[m_selectedIndex[i]]);
        std::string prefixString = filePrefix.toStdString() ;
        prefixString = prefixString.erase( prefixString.find_last_not_of( " \n\r\t") + 1 ) ;
        exporter->SetFilePrefix(prefixString.c_str());
        exporter->Write();
    }

    exporter->Delete();
}
#endif

void ShapePopulationQT::showNoExportWindow()
{
    std::ostringstream strs;
    strs << "GL2PS is not available in VTK's version of 3D Slicer." << std::endl << std::endl
         << "To export your windows in high-resolution files, you can use the full "
         << "version of ShapePopulationViewer, available on its NITRC webpage"<< std::endl
         << "http://www.nitrc.org/projects/shapepopviewer"<< std::endl;

    QMessageBox::about(this,"Not available in Slicer",QString(strs.str().c_str()));
}
