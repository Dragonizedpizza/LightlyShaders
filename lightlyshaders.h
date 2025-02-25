/*
 *   Copyright © 2015 Robert Metsäranta <therealestrob@gmail.com>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *   General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; see the file COPYING.  if not, write to
 *   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *   Boston, MA 02110-1301, USA.
 */

#ifndef LIGHTLYSHADERS_H
#define LIGHTLYSHADERS_H

#include <kwineffects.h>

namespace KWin {

class GLTexture;

class Q_DECL_EXPORT LightlyShadersEffect : public Effect

{
    Q_OBJECT
public:
    LightlyShadersEffect();
    ~LightlyShadersEffect();

    static bool supported();
    static bool enabledByDefault();
    
    void setRoundness(const int r, EffectScreen *s);
    void reconfigure(ReconfigureFlags flags) override;
    void paintScreen(int mask, const QRegion &region, ScreenPaintData &data) override;
    void prePaintWindow(EffectWindow* w, WindowPrePaintData& data, std::chrono::milliseconds time) override;
    void paintWindow(EffectWindow* w, int mask, QRegion region, WindowPaintData& data) override;
    virtual int requestedEffectChainPosition() const override { return 99; }

    enum { RoundedCorners = 0, SquircledCorners };

protected Q_SLOTS:
    void windowAdded(EffectWindow *window);
    void windowDeleted(EffectWindow *window);
    void windowMaximizedStateChanged(EffectWindow *window, bool horizontal, bool vertical);
    void windowUnminimized(EffectWindow *window);

private:
    enum { TopLeft = 0, TopRight, BottomRight, BottomLeft, NTex };
    enum { Top = 0, Bottom, NShad };

    struct LSWindowStruct
    {
        bool updateDiffTex;
        bool skipEffect;
        bool hasFadeInAnimation;
        bool hasRestoreAnimation;
        bool isManaged;
        QRegion clip;
        QMap<EffectScreen *, QList<GLTexture>> diffTextures;
        std::chrono::milliseconds animationTime;
    };

    struct LSScreenStruct
    {
        qreal scale=1.0;
        int sizeScaled;
        GLTexture *tex[NTex];
        GLTexture *rect[NTex];
        GLTexture *darkRect[NTex];
    };

    void genMasks(EffectScreen *s);
    void genRect(EffectScreen *s);

    bool isValidWindow(EffectWindow *w, int mask=0);

    void fillRegion(const QRegion &reg, const QColor &c);
    GLTexture copyTexSubImage(const QRect &geo, const QRect &rect, qreal xTranslation=0.0, qreal yTranslation=0.0);
    QList<GLTexture> getTexRegions(EffectWindow *w, const QRect* rect, const QRect &geo, int nTex, qreal xTranslation=0.0, qreal yTranslation=0.0, bool force=false);
    void drawSquircle(QPainter *p, float size, int translate);
    QImage genMaskImg(int size, bool mask, bool outer_rect);
    void getShadowDiffs(EffectWindow *w, const QRect* rect, QList<GLTexture> &emptyCornersTextures, qreal xTranslation=0.0, qreal yTranslation=0.0, bool outOfScreen=false, int w_mask=0);
    QRect scale(const QRect rect, qreal scaleFactor);

    int m_size, m_alpha, m_cornersType, m_squircleRatio, m_roundness, m_shadowOffset;
    bool m_outline, m_darkTheme, m_disabledForMaximized;
    GLShader *m_shader, *m_diffShader;
    QSize m_corner;
    qreal m_zoom=1.0, m_xTranslation=0, m_yTranslation=0;

    QMap<EffectScreen *, LSScreenStruct> m_screens;
    QMap<EffectWindow *, LSWindowStruct> m_windows;
};

} // namespace KWin

#endif //LIGHTLYSHADERS_H

