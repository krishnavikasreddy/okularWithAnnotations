// fontpool.h
//
// (C) 2001-2003 Stefan Kebekus
// Distributed under the GPL

#ifndef _FONTPOOL_H
#define _FONTPOOL_H

#include <../config.h>
#ifdef HAVE_FREETYPE
#include <ft2build.h>
#include FT_FREETYPE_H
#endif
#include <qmap.h>
#include <qptrlist.h>
#include <qstringlist.h>
#include <qobject.h>

#include "TeXFontDefinition.h"

class KProcess;
class KShellProcess;

#define NumberOfMFModes 3
#define DefaultMFMode 1

extern const char *MFModes[];
extern const char *MFModenames[];
extern const int   MFResolutions[];


/**
 *  A list of fonts and a compilation of utility functions
 *
 * This class holds a list of fonts and is able to perform a number of
 * functions on each of the fonts. The main use of this class is that
 * it is able to control a concurrently running "kpsewhich" programm
 * which is used to locate and load the fonts.
 *
 * @author Stefan Kebekus   <kebekus@kde.org>
 *
 **/

class fontPool : public QObject {
Q_OBJECT

 public:
  /** Default constructor. */
  fontPool( void );

  /** Default destructor. */
  ~fontPool( void );

  /** Method used to set the MetafontMode for the PK font files. This
      data is used when loading fonts. Currently, a change here will
      be applied only to those font which were not yet loaded
      ---expect funny results when changing the data in the
      mid-work. The integer argument must be smaller than
      NumberOfMFModes, which is defined in fontpool.h and refers to a
      Mode/Resolution pair in the lists MFModes and MFResolutins which
      are also defined in fontpool.h and implemented in
      fontpool.cpp. Returns the mode number of the mode which was
      actually set ---if an invalid argument is given, this will be
      the DefaultMFMode as defined in fontPool.h */
  //  unsigned int setMetafontMode( unsigned int );

  void setParameters( unsigned int MetaFontMode, bool makePK, bool enlargeFonts, bool useType1Fonts, bool useFontHints );

  /** Returns the currently set MetafontMode */

  unsigned int getMetafontMode(void) {return MetafontMode;};

  /** Sets whether fonts will be generated by running MetaFont, or a
      similar programm. If (flag == 0), fonts will not be generated,
      otherwise they will. */
  //  void setMakePK( bool flag );

  /** Sets whether glyphs will be enlarged or not. If (flag == 0),
      glyphs will not be enlarged, otherwise they will. */
  //  void setEnlargeFonts( bool flag );

  /** Sets the resolution of the output device. */
  void setDisplayResolution( double _displayResolution_in_dpi );

  /** Sets the resolution of the output device and determines if font hinting should be used. */
  //  void setUseFontHints( bool _useFontHinting );

  /** Determines if Adobe Type 1 (*.pfb) fonts should be used or not */
  //  void setUseType1Fonts( bool _useType1Fonts );

  /** Returns the status of enlargeFonts.  If the return value is ==
      0, glyphs will not be enlarged, otherwise they will. */
  bool getEnlargeFonts() {return enlargeFonts;};

  /** If return value is true, font hinting should be used if
      possible */
  bool getUseFontHints(void) {return useFontHints;};

  /** If return value is true, Type 1 fonts will be used, if available */
  bool getUseType1Fonts(void) {return useType1Fonts;};

  /** This method adds a font to the list. If the font is not
      currently loaded, it's file will be located and font::load_font
      will be called. Since this is done using a concurrently running
      process, there is no guarantee that the loading is already
      performed when the method returns. */
  TeXFontDefinition *appendx(QString fontname, Q_UINT32 checksum, Q_UINT32 scale, double enlargement);

  /** Prints very basic debugging information about the fonts in the
      pool to the kdDebug output stream. */
  QString status();

  /** Checks if all the fonts file names have been looked at, and
      returns true if that is so. Otherwise, the method starts
      kpsewhich in a concurrent process and returns false. Once the
      kpsewhich is terminated, the signal fonts_info is emitted. */
  bool check_if_fonts_filenames_are_looked_up(void);
  
  /** This is the list which actually holds pointers to the fonts in
      the fontPool */
  QPtrList<TeXFontDefinition> fontList;

  /** This method marks all fonts in the fontpool as "not in use". The
      fonts are, however, not removed from memory until the method
      release_fonts is called. The method is called when the dvi-file
      is closed. Because the next dvi-file which will be loaded is
      likely to use most of the fonts again, this method implements a
      convenient way of re-using fonts without loading them
      repeatedly. */
  void mark_fonts_as_unused(void);

  /** This methods removes all fonts from the fontpool (and thus from
      memory) which are labeled "not in use". For explanation, see the
      mark_fonts_as_unused method. */
  void release_fonts(void);

#ifdef HAVE_FREETYPE
  /** A handle to the FreeType library, which is used by TeXFont_PFM
      font objects, if KDVI is compiled with FreeType support.  */
  FT_Library FreeType_library;
  
  /** Simple marker. Set to 'true', if the FreeType library was loaded
      successfully */
  bool FreeType_could_be_loaded;

  /** This maps TeX font names to actual filenames that we can look up
      with kpsewhich. Unfortunately, this is necessary for a number of
      Type1 fonts, such as URWBookmanL-DemiBold whose TeX name is
      'rpbkd' but which is contained in the file 'ubkd8a.pfb'. The
      contents of 'ps2pk.map' is parsed to set up this map. */
  QMap<QString, QString> fontFilenames;
#endif


signals:
  /** Emitted to indicate that the progress dialog should show up now. */
  void show_progress(void);

  /** Emitted to indicate that all the fonts have now been loaded so
      that the first page can be rendered. */
  void fonts_have_been_loaded(fontPool *);

  /** The title says it all... */
  void hide_progress_dialog(void);

  /** Emitted at the start of a kpsewhich job to indicate how many
      fonts there are to be looked up/generated. */
  void totalFontsInJob(int);

  /** Emitted during the kpsewhich job to give the total number of
    fonts which have been looked up/generated. */
  void numFoundFonts(int);

  /** Emitted to pass output of MetaFont and friends on to the user
      interface. */
  void MFOutput(QString);

  /** Emitted when a kpsewhich run is started in order to clear the
      info dialog window. */
  void new_kpsewhich_run(QString);

  /** Passed through to the top-level kpart. */
  void setStatusBarText( const QString& );

public slots:
  /** Aborts the font generation. Nasty. */
  void abortGeneration(void);

  /** For internal purposes only. This slot is called when the
      kpsewhich program has terminated. */
 void kpsewhich_terminated(KProcess *);
 
 /** For internal purposess only. This slot is called when MetaFont is
     run via the kpsewhich programm. The MetaFont output is transmitted
     to the fontpool via this slot. */
 void mf_output_receiver(KProcess *, char *, int);
 
 /** For internal purposess only. This slot is called when kpsewhich
     outputs the name of a font which has been found or was
     generated. */
 void kpsewhich_output_receiver(KProcess *, char *, int);
 
 private:
  /** Runs kpsewhich in a concurrent process. The type of fonts which
      is looked for is determined by the variable 'pass', and is
      explained below. NOTE: 'pass' MUST BE SET TO A MEANINGFUL VALUE
      BEFORE THIS METHOD IS CALLED. */
  void start_kpsewhich(void);

  /** This variable MUST be set before the method start_kpsewhich() is
      called. It determines what kind of font files kpsewhich is told
      to look for.
      
      0: concurrently runnign kpsewhich program looks for pk-fonts and
      virtual fonts, and disables automatic pk generation, even if it
      is turned on with the setMakePK-method. After the kpsewhich
      process terminates, the method kpsewhich_terminated() is called,
      sets pass to 1, and calls start_kpsewhich() again.
      
      1: kpsewhich will look only for pk fonts are looked for, and
      fonts are generated, if necessary. After the kpsewhich process
      terminates, the method kpsewhich_terminated() will check if all
      font filenames are there. If so, the signal If not, a warning dialog is shown,
      pass is set to 2, and start_kpsewhich() is called again.
      
      2: kpsewhich will look for TFM files, which are used as a last
      resort. After the kpsewhich process terminates, the signals
      setStatusBarText(QString::null) and fonts_have_been_loaded()
      will be emitted. */
  Q_UINT8 pass;

 /** This flag determines whether we try to have MetaFont generate the
     font if a bitmap is not available. If makepk == 0, then bitmaps
     are NOT generated. */
 bool           makepk;
 
 /** This flag determines whether the glyphs should be enlarged by 10%
     or not. If enlargeFonts==0, then fonts are NOT enlarged. */
 bool           enlargeFonts;
 
 /** This flag is used by PFB fonts to determine if the FREETYPE
     engine should use hinted fonts or not */
 bool           useFontHints; 

 /** This flag is used to determine Type1 fonts should be loaded at
     all or not */
 bool           useType1Fonts; 

 /** This integer determines the Metafont-Mode which is used in
     generating bitmaps. The value must satisfy 0 <= MetafontMode <
     NumberOfMFModes, and refers to an entry in the lists
     MFModenames/MFModes and MFResolutions which are defined in
     fontpool.cpp */
 unsigned int   MetafontMode;
 
 /** Resolution of the output device. */
 double         displayResolution_in_dpi;
 
 /** This QString is used to collect the output of the MetaFont
     programm while we are waiting for a full screen line */
 QString        MetafontOutput;
 
 QString        kpsewhichOutput;

 /** This is the ShellProcess which is used to run the kpsewhich
     command which locates the font-files for us. */
 KShellProcess *proc;
 
 /** FontProgress; the progress dialog used when generating fonts. */
 class fontProgressDialog *progress;
};

#endif //ifndef _FONTPOOL_H
