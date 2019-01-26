#include "Thumbnails.h"

#include <EditorLib/LogFile.h>

#include <QImage>
#include <QDir>

#ifdef WIN32
#include <thumbcache.h>
#endif


#ifdef WIN32
QPixmap fromWinHBITMAP(HBITMAP bitmap);
#endif

#define THUMB_SIZE 64

struct Thumbnails::Opaque
{
#ifdef WIN32
    IThumbnailCache* cache_ = 0x0;
#endif
};

Thumbnails::Thumbnails() :
    privateData_(new Thumbnails::Opaque())
{
#ifdef WIN32
    HRESULT hr = CoCreateInstance(CLSID_LocalThumbnailCache, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&privateData_->cache_));
    if (SUCCEEDED(hr))
        LOGINFO("Successfully created ThumbnailCache COM interface");
    else
    {
        LOGERROR("Failed to open IThumbnailCache COM interface.");
        privateData_->cache_ = 0x0;
    }
#endif
}

Thumbnails::~Thumbnails()
{
#ifdef WIN32
    if (privateData_->cache_)
        privateData_->cache_->Release();
#endif
    delete privateData_;
}

QPixmap Thumbnails::GetPixmap(const QString& filePath) const
{
    QPixmap ret;
#ifdef WIN32
    if (privateData_->cache_)
    {
        IShellItem* pItem = 0x0;
        QString fp = QDir::toNativeSeparators(filePath);
        std::wstring str = fp.toStdWString();
        HRESULT hr = ::SHCreateItemFromParsingName(str.c_str(), 0x0, IID_PPV_ARGS(&pItem));
        if (SUCCEEDED(hr))
        {
            ISharedBitmap *pBitmap;
            hr = privateData_->cache_->GetThumbnail(pItem, THUMB_SIZE, WTS_SCALETOREQUESTEDSIZE, &pBitmap, NULL, NULL);
            if (SUCCEEDED(hr))
            {
                HBITMAP hBitmap;
                hr = pBitmap->GetSharedBitmap(&hBitmap);
                ret = fromWinHBITMAP(hBitmap);

                pBitmap->Release();
            }
            pItem->Release();
        }
    }
#endif
    return ret;
}


#ifdef WIN32
QPixmap fromWinHBITMAP(HBITMAP bitmap)
{
    // Verify size
    BITMAP bitmap_info;
    memset(&bitmap_info, 0, sizeof(BITMAP));

    int res;
    //QT_WA({
    res = GetObjectW(bitmap, sizeof(BITMAP), &bitmap_info);
    //}, {
    //    res = GetObjectA(bitmap, sizeof(BITMAP), &bitmap_info);
    //});
    //
    if (!res) {
        LOGERROR("Failed to get bitmap info for thumbnail conversion");
        return QPixmap();
    }
    int w = bitmap_info.bmWidth;
    int h = bitmap_info.bmHeight;

    BITMAPINFO bmi;
    memset(&bmi, 0, sizeof(bmi));
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = w;
    bmi.bmiHeader.biHeight = -h;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    bmi.bmiHeader.biSizeImage = w * h * 4;

    QImage result;
    // Get bitmap bits
    uchar *data = (uchar *)malloc(bmi.bmiHeader.biSizeImage);
    if (GetDIBits(qt_win_display_dc(), bitmap, 0, h, data, &bmi, DIB_RGB_COLORS)) {

        QImage::Format imageFormat = QImage::Format_ARGB32_Premultiplied;
        uint mask = 0;
        //if (format == NoAlpha) {
        imageFormat = QImage::Format_RGB32;
        mask = 0xff000000;
        //}

        // Create image and copy data into image.
        QImage image(w, h, imageFormat);
        if (!image.isNull()) { // failed to alloc?
            int bytes_per_line = w * sizeof(QRgb);
            for (int y = 0; y<h; ++y) {
                QRgb *dest = (QRgb *)image.scanLine(y);
                const QRgb *src = (const QRgb *)(data + y * bytes_per_line);
                for (int x = 0; x<w; ++x) {
                    dest[x] = src[x] | mask;
                }
            }
        }
        result = image;
    }
    else {
        qWarning("QPixmap::fromWinHBITMAP(), failed to get bitmap bits");
    }
    free(data);
    return QPixmap::fromImage(result);
}
#endif

QIcon Thumbnails::icon(IconType type) const
{
    return QFileIconProvider::icon(type);
}

QIcon Thumbnails::icon(const QFileInfo &info) const
{
    QString path = info.absoluteFilePath();
    QPixmap pix = GetPixmap(path);
    if (pix.size().isEmpty())
        return QFileIconProvider::icon(info);
    return QIcon(GetPixmap(path));
}