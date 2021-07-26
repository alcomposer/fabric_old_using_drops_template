/*
    Drops - Drops Really Only Plays Samples
    Copyright (C) 2021  Rob van den Berg

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
#include "DropsUI.hpp"

START_NAMESPACE_DISTRHO

void DropsUI::initTabAmp()
{
    Window &window = getParentWindow();
    const float margin = 2.f;
    const float gauge = 6.f;
    const float font_size = 14.f;
    const uint sz = 60;
    const Size<uint> knobSize = Size<uint>(sz + font_size, sz);

    vbox_amp = new VBox(window);
    vbox_amp->setId(kVBoxAmp);
    vbox_amp->setAbsolutePos(12, 329);
    vbox_amp->setWidth(323);
    vbox_amp->setHeight(211);

    hbox_amp_row_1 = new HBox(vbox_amp);
    hbox_amp_row_1->setId(kHBoxAmpRow1);
    hbox_amp_row_1->setWidth(vbox_amp->getWidth() * 0.8);
    hbox_amp_row_1->setHeight(sz);

    hbox_amp_row_2 = new HBox(vbox_amp);
    hbox_amp_row_2->setId(kHBoxAmpRow2);
    hbox_amp_row_2->setHeight(sz);
    hbox_amp_row_2->setWidth(vbox_amp->getWidth() * 0.9);

    //fAmpEgAttack = new Knob(hbox_amp_row_1);
    //fAmpEgAttack->setId(kAmpEgAttack);
    //fAmpEgAttack->setSize(knobSize);
    //fAmpEgAttack->setCallback(this);
    //fAmpEgAttack->labelSize = font_size;
    //fAmpEgAttack->gauge_width = gauge;
    //fAmpEgAttack->margin = margin;
    //fAmpEgAttack->label = "ATTACK";
    //fAmpEgAttack->foreground_color = saffron;
    //fAmpEgAttack->background_color = black_olive;
    //fAmpEgAttack->highlight_color = saffron_1;
    //fAmpEgAttack->text_color = floral_white;
    //fAmpEgAttack->real_min = 0.0f;
    //fAmpEgAttack->real_max = 10.0f;
    //fAmpEgAttack->min = 1.0f;
    //fAmpEgAttack->max = 101.f;
    //fAmpEgAttack->format_str = "%.2f s";
    //fAmpEgAttack->using_log = true;
    //fAmpEgAttack->setParamOnMove = true;

    fPlayheadSpeed = new Knob(hbox_amp_row_1);
    fPlayheadSpeed->setId(kPlayheadSpeed);
    fPlayheadSpeed->setSize(knobSize);
    fPlayheadSpeed->setCallback(this);
    fPlayheadSpeed->labelSize = font_size;
    fPlayheadSpeed->gauge_width = gauge;
    fPlayheadSpeed->margin = margin;
    fPlayheadSpeed->label = "PLAY SPEED";
    fPlayheadSpeed->foreground_color = saffron;
    fPlayheadSpeed->background_color = black_olive;
    fPlayheadSpeed->highlight_color = saffron_1;
    fPlayheadSpeed->text_color = floral_white;
    fPlayheadSpeed->real_min = -1.0f;
    fPlayheadSpeed->real_max = 1.f;
    fPlayheadSpeed->min = -1.0f;
    fPlayheadSpeed->max = 1.f;
    fPlayheadSpeed->format_str = "%.2f";
    //fAmpEgAttack->setParamOnMove = true;

    fGrainDensity = new Knob(hbox_amp_row_1);
    fGrainDensity->setId(kGrainDensity);
    fGrainDensity->setSize(knobSize);
    fGrainDensity->setCallback(this);
    fGrainDensity->labelSize = font_size;
    fGrainDensity->gauge_width = gauge;
    fGrainDensity->margin = margin;
    fGrainDensity->label = "DENSITY";
    fGrainDensity->foreground_color = saffron;
    fGrainDensity->background_color = black_olive;
    fGrainDensity->highlight_color = saffron_1;
    fGrainDensity->text_color = floral_white;  
    fGrainDensity->real_min = 0.0f;
    fGrainDensity->real_max = 1000.f;
    fGrainDensity->min = 1.0f;
    fGrainDensity->max = 1000.f;
    fGrainDensity->format_str = "%.2f Hz";
    fGrainDensity->using_log = true;
    fGrainDensity->setParamOnMove = true;

    fGrainLength = new Knob(hbox_amp_row_1);
    fGrainLength->setId(kGrainLength);
    fGrainLength->setSize(knobSize);
    fGrainLength->setCallback(this);
    fGrainLength->labelSize = font_size;
    fGrainLength->gauge_width = gauge;
    fGrainLength->margin = margin;
    fGrainLength->label = "LENGTH";
    fGrainLength->foreground_color = saffron;
    fGrainLength->background_color = black_olive;
    fGrainLength->highlight_color = saffron_1;
    fGrainLength->text_color = floral_white;
    fGrainLength->real_min = 0.0f;
    fGrainLength->real_max = 10.f;
    fGrainLength->min = 1.0f;
    fGrainLength->max = 101.f;
    fGrainLength->format_str = "%.2f s";
    fGrainLength->using_log = true;
    fGrainLength->setParamOnMove = true;

    fSpray = new Knob(hbox_amp_row_1); // was kAmpLFOFade
    fSpray->setId(kSpray);
    fSpray->setCallback(this);
    fSpray->setSize(knobSize);
    fSpray->labelSize = font_size;
    fSpray->gauge_width = gauge;
    fSpray->margin = margin;
    fSpray->label = "SPRAY";
    fSpray->background_color = black_olive;
    fSpray->foreground_color = saffron;
    fSpray->highlight_color = saffron_1;
    fSpray->text_color = floral_white;
    fSpray->default_value = 0.0f;
    fSpray->real_min = 0.0f;
    fSpray->real_max = 10.0f;
    fSpray->format_str = "%.2f sec";
    fSpray->setParamOnMove = true;

    hbox_amp_row_1->addWidget(fPlayheadSpeed);
    hbox_amp_row_1->addWidget(fGrainDensity);
    hbox_amp_row_1->addWidget(fGrainLength);
    hbox_amp_row_1->addWidget(fSpray);

    // row 2

    fAmpLFOType = new DropDown(hbox_amp_row_2);
    fAmpLFOType->setId(kAmpLFOType);
    fAmpLFOType->font_size = font_size + 2;
    fAmpLFOType->setFont("Roboto_Regular",
                         reinterpret_cast<const uchar *>(fonts::Roboto_RegularData),
                         fonts::Roboto_RegularDataSize);
    fAmpLFOType->setMenuFont("DropsIcons",
                             reinterpret_cast<const uchar *>(fonts::drops_iconsData),
                             fonts::drops_iconsDataSize);
    fAmpLFOType->setSize(200,
                         fAmpLFOType->font_size + fAmpLFOType->margin * 2.0f);
    fAmpLFOType->setCallback(this);
    fAmpLFOType->label = "TYPE :";
    fAmpLFOType->item = "a";
    fAmpLFOType->foreground_color = saffron;
    fAmpLFOType->background_color = black_olive;
    fAmpLFOType->text_color = floral_white;

    fAmpLFOSync = new CheckBox(hbox_amp_row_2);
    fAmpLFOSync->setId(kAmpLFOSync);
    fAmpLFOSync->setSize(knobSize.getWidth()*.5,knobSize.getHeight());
    fAmpLFOSync->setCallback(this);
    fAmpLFOSync->background_color = black_olive;
    fAmpLFOSync->foreground_color = saffron;
    fAmpLFOSync->highlight_color_active = saffron_1;
    fAmpLFOSync->highlight_color_inactive = black_olive_1;
    fAmpLFOSync->text_color = floral_white;
    fAmpLFOSync->setFont("Roboto_Regular",
                         reinterpret_cast<const uchar *>(fonts::Roboto_RegularData),
                         fonts::Roboto_RegularDataSize);
    fAmpLFOSync->margin = 6.f;
    fAmpLFOSync->label = "REC";
    fAmpLFOSync->boxSize = 20;
    fAmpLFOSync->labelSize = font_size;

    fAmpLFOFreq = new Knob(hbox_amp_row_2);
    fAmpLFOFreq->setId(kAmpLFOFreq);
    fAmpLFOFreq->setCallback(this);
    fAmpLFOFreq->setSize(knobSize);
    fAmpLFOFreq->labelSize = font_size;
    fAmpLFOFreq->gauge_width = gauge;
    fAmpLFOFreq->margin = margin;
    fAmpLFOFreq->label = "";
    fAmpLFOFreq->background_color = black_olive;
    fAmpLFOFreq->foreground_color = saffron;
    fAmpLFOFreq->highlight_color = saffron_1;
    fAmpLFOFreq->text_color = floral_white;
    fAmpLFOFreq->default_value = 500.f;
    fAmpLFOFreq->real_min = 1.0f;
    fAmpLFOFreq->real_max = 1000.f;
    fAmpLFOFreq->min = 1.0f;
    fAmpLFOFreq->max = 1000.0f;
    fAmpLFOFreq->format_str = "%.2f Hz";
    fAmpLFOFreq->using_log = false;
    fAmpLFOFreq->setParamOnMove = true;


    fAmpLFOTypeMenu = new Menu(hbox_amp_row_2);
    fAmpLFOTypeMenu->setMaxViewItems(6);
    fAmpLFOTypeMenu->setId(kAmpLFOTypeMenu);
    fAmpLFOTypeMenu->setCallback(this);
    fAmpLFOTypeMenu->font_size = font_size + 2;
    fAmpLFOTypeMenu->setFont("Drops Icons",
                             reinterpret_cast<const uchar *>(fonts::drops_iconsData),
                             fonts::drops_iconsDataSize);
    fAmpLFOTypeMenu->addItems({"a",
                               "b",
                               "c",
                               "d",
                               "e",
                               "f"});
    fAmpLFOTypeMenu->hide();
    fAmpLFOTypeMenu->background_color = black_olive;
    fAmpLFOTypeMenu->foreground_color = black_olive_2;
    fAmpLFOTypeMenu->highlight_color = saffron_1;
    fAmpLFOTypeMenu->text_color = floral_white;

    fAmpLFOType->setMenu(fAmpLFOTypeMenu);
    fAmpLFOType->resize();

    hbox_amp_row_2->addWidget(fAmpLFOType);
    hbox_amp_row_2->addWidget(fAmpLFOSync);
    hbox_amp_row_2->addWidget(fAmpLFOFreq);

    vbox_amp->addWidget(hbox_amp_row_1);
    vbox_amp->addWidget(hbox_amp_row_2);

    vbox_amp->setWidgetResize(kHBoxAmpRow1, true);
    vbox_amp->setWidgetResize(kHBoxAmpRow2, true);

    vbox_amp->positionWidgets();
    hbox_amp_row_1->positionWidgets();
    hbox_amp_row_2->positionWidgets();

    fAmpLFOType->positionMenu();
}

END_NAMESPACE_DISTRHO