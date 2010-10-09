/* This file is part of Clementine.

   Clementine is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   Clementine is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Clementine.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "collapsibleinfoheader.h"
#include "collapsibleinfopane.h"

#include <QVBoxLayout>

CollapsibleInfoPane::CollapsibleInfoPane(const Data& data, QWidget* parent)
  : QWidget(parent),
    data_(data),
    header_(new CollapsibleInfoHeader(this))
{
  QVBoxLayout* layout = new QVBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(3);
  layout->setSizeConstraint(QLayout::SetMinAndMaxSize);
  setLayout(layout);

  layout->addWidget(header_);
  layout->addWidget(data.contents_);

  header_->SetTitle(data.title_);
  header_->SetIcon(data.icon_);

  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);

  connect(header_, SIGNAL(ExpandedToggled(bool)), SLOT(ExpandedToggled(bool)));
}

void CollapsibleInfoPane::Collapse() {
  header_->SetExpanded(false);
}

void CollapsibleInfoPane::Expand() {
  header_->SetExpanded(true);
}

void CollapsibleInfoPane::ExpandedToggled(bool expanded) {
  data_.contents_->setVisible(expanded);
}

bool CollapsibleInfoPane::Data::operator <(const CollapsibleInfoPane::Data& other) const {
  const int my_score    = type_       * 1000 + relevance_;
  const int other_score = other.type_ * 1000 + other.relevance_;

  return my_score > other_score;
}